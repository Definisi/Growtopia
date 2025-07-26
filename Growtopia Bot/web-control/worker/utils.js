const { PrismaClient } = require('@prisma/client');
const axios = require('axios');
const config = require('./config');
const logger = require('./logger');

class DatabaseUtils {
    constructor() {
        this.prisma = new PrismaClient();
    }

    async connect() {
        try {
            await this.prisma.$connect();
            logger.success('Database connected successfully');
            return true;
        } catch (error) {
            logger.error('Failed to connect to database:', error.message);
            return false;
        }
    }

    async disconnect() {
        try {
            await this.prisma.$disconnect();
            logger.info('Database disconnected');
        } catch (error) {
            logger.error('Error disconnecting from database:', error.message);
        }
    }

    async getAllNodes() {
        try {
            const nodes = await this.prisma.node.findMany({
                include: {
                    bots: true
                }
            });
            return nodes;
        } catch (error) {
            logger.error('Failed to get nodes from database:', error.message);
            return [];
        }
    }

    async updateNodeStatus(nodeId, isActive) {
        try {
            await this.prisma.node.update({
                where: { id: nodeId },
                data: { 
                    status: isActive ? 'online' : 'offline',
                    lastSeen: new Date()
                }
            });
            logger.debug(`Node ${nodeId} status updated to ${isActive ? 'online' : 'offline'}`);
        } catch (error) {
            logger.error(`Failed to update node ${nodeId} status:`, error.message);
        }
    }

    async removeBotsFromInactiveNode(nodeId) {
        try {
            const result = await this.prisma.bot.deleteMany({
                where: { nodeId }
            });
            logger.info(`Removed ${result.count} bots from inactive node ${nodeId}`);
            return result.count;
        } catch (error) {
            logger.error(`Failed to remove bots from node ${nodeId}:`, error.message);
            return 0;
        }
    }

    async syncBotData(nodeId, activeBots) {
        try {
            // Get current bots in database for this node
            const dbBots = await this.prisma.bot.findMany({
                where: { nodeId }
            });

            const dbBotIds = new Set(dbBots.map(bot => bot.id));
            const activeBotIds = new Set(activeBots.map(bot => bot.id));

            // Remove bots that are in DB but not active
            const botsToRemove = dbBots.filter(bot => !activeBotIds.has(bot.id));
            for (const bot of botsToRemove) {
                await this.prisma.bot.delete({
                    where: { id: bot.id }
                });
                logger.debug(`Removed inactive bot ${bot.id} from database`);
            }

            // Add or update active bots
            for (const bot of activeBots) {
                if (dbBotIds.has(bot.id)) {
                    // Update existing bot
                    await this.prisma.bot.update({
                        where: { id: bot.id },
                        data: {
                            status: bot.status,
                            lastSeen: new Date(),
                            ...bot
                        }
                    });
                    logger.debug(`Updated bot ${bot.id}`);
                } else {
                    // Add new bot
                    await this.prisma.bot.create({
                        data: {
                            ...bot,
                            nodeId,
                            lastSeen: new Date()
                        }
                    });
                    logger.debug(`Added new bot ${bot.id}`);
                }
            }

            logger.info(`Synced ${activeBots.length} bots for node ${nodeId}`);
            return {
                added: activeBots.filter(bot => !dbBotIds.has(bot.id)).length,
                updated: activeBots.filter(bot => dbBotIds.has(bot.id)).length,
                removed: botsToRemove.length
            };
        } catch (error) {
            logger.error(`Failed to sync bot data for node ${nodeId}:`, error.message);
            return { added: 0, updated: 0, removed: 0 };
        }
    }

    async getStats() {
        try {
            const [totalNodes, activeNodes, totalBots, activeBots] = await Promise.all([
                this.prisma.node.count(),
                this.prisma.node.count({ where: { status: 'online' } }),
                this.prisma.bot.count(),
                this.prisma.bot.count({ where: { status: 'online' } })
            ]);

            return {
                nodes: { total: totalNodes, active: activeNodes },
                bots: { total: totalBots, active: activeBots }
            };
        } catch (error) {
            logger.error('Failed to get database stats:', error.message);
            return {
                nodes: { total: 0, active: 0 },
                bots: { total: 0, active: 0 }
            };
        }
    }
}

class NetworkUtils {
    static async checkNodeHealth(nodeUrl) {
        try {
            const response = await axios.get(`${nodeUrl}${config.api.endpoints.health}`, {
                timeout: config.timeouts.node,
                headers: {
                    'User-Agent': 'Growtopia-Bot-Worker/1.0'
                }
            });
            return response.status === 200;
        } catch (error) {
            logger.debug(`Node health check failed for ${nodeUrl}:`, error.message);
            return false;
        }
    }

    static async getActiveBots(nodeUrl) {
        try {
            const response = await axios.get(`${nodeUrl}${config.api.endpoints.bots}`, {
                timeout: config.timeouts.botApi,
                headers: {
                    'User-Agent': 'Growtopia-Bot-Worker/1.0'
                }
            });
            
            if (response.status === 200 && Array.isArray(response.data)) {
                return response.data;
            }
            return [];
        } catch (error) {
            logger.debug(`Failed to get bots from ${nodeUrl}:`, error.message);
            return [];
        }
    }

    static async testConnection(url) {
        try {
            const response = await axios.get(url, {
                timeout: 3000,
                headers: {
                    'User-Agent': 'Growtopia-Bot-Worker/1.0'
                }
            });
            return { success: true, status: response.status };
        } catch (error) {
            return { 
                success: false, 
                error: error.message,
                code: error.code
            };
        }
    }
}

class ValidationUtils {
    static isValidNodeUrl(url) {
        try {
            const parsed = new URL(url);
            return ['http:', 'https:'].includes(parsed.protocol);
        } catch {
            return false;
        }
    }

    static isValidBotData(bot) {
        return (
            bot &&
            typeof bot.id === 'string' &&
            bot.id.length > 0 &&
            typeof bot.status === 'string' &&
            Object.values(config.botStatus).includes(bot.status)
        );
    }

    static sanitizeBotData(bot) {
        return {
            id: String(bot.id || '').trim(),
            status: String(bot.status || 'offline').toLowerCase(),
            username: String(bot.username || '').trim(),
            world: String(bot.world || '').trim(),
            level: parseInt(bot.level) || 0,
            gems: parseInt(bot.gems) || 0,
            lastActivity: bot.lastActivity ? new Date(bot.lastActivity) : new Date()
        };
    }
}

module.exports = {
    DatabaseUtils,
    NetworkUtils,
    ValidationUtils
};