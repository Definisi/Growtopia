const { DatabaseUtils, NetworkUtils } = require('./utils');
const logger = require('./logger');
const config = require('./config');

class HealthChecker {
    constructor() {
        this.dbUtils = new DatabaseUtils();
    }

    async checkDatabaseHealth() {
        try {
            const connected = await this.dbUtils.connect();
            if (!connected) {
                return { status: 'error', message: 'Database connection failed' };
            }

            // Test basic database operations
            const stats = await this.dbUtils.getStats();
            await this.dbUtils.disconnect();

            return {
                status: 'healthy',
                message: 'Database is operational',
                stats
            };
        } catch (error) {
            logger.error('Database health check failed:', error.message);
            return {
                status: 'error',
                message: `Database error: ${error.message}`,
                error: error.code || 'UNKNOWN'
            };
        }
    }

    async checkNodeConnectivity(nodes) {
        const results = [];
        
        for (const node of nodes) {
            const nodeUrl = `http://${node.host}:${node.port}`;
            const isHealthy = await NetworkUtils.checkNodeHealth(nodeUrl);
            
            results.push({
                id: node.id,
                name: node.name,
                url: nodeUrl,
                status: isHealthy ? 'healthy' : 'unreachable',
                lastChecked: new Date().toISOString()
            });
        }

        return results;
    }

    async performFullHealthCheck() {
        logger.info('Starting comprehensive health check...');
        
        const results = {
            timestamp: new Date().toISOString(),
            database: await this.checkDatabaseHealth(),
            nodes: [],
            summary: {
                healthy: true,
                issues: []
            }
        };

        // Check database first
        if (results.database.status !== 'healthy') {
            results.summary.healthy = false;
            results.summary.issues.push(`Database: ${results.database.message}`);
        }

        // Check nodes if database is healthy
        if (results.database.status === 'healthy') {
            try {
                await this.dbUtils.connect();
                const nodes = await this.dbUtils.getAllNodes();
                results.nodes = await this.checkNodeConnectivity(nodes);
                await this.dbUtils.disconnect();

                const unhealthyNodes = results.nodes.filter(n => n.status !== 'healthy');
                if (unhealthyNodes.length > 0) {
                    results.summary.healthy = false;
                    results.summary.issues.push(`${unhealthyNodes.length} node(s) unreachable`);
                }
            } catch (error) {
                results.summary.healthy = false;
                results.summary.issues.push(`Node check failed: ${error.message}`);
            }
        }

        logger.info(`Health check completed. Status: ${results.summary.healthy ? 'HEALTHY' : 'ISSUES DETECTED'}`);
        if (!results.summary.healthy) {
            logger.warn('Issues found:', results.summary.issues.join(', '));
        }

        return results;
    }

    async waitForDatabaseReady(maxAttempts = 10, delayMs = 2000) {
        for (let attempt = 1; attempt <= maxAttempts; attempt++) {
            logger.info(`Database readiness check (${attempt}/${maxAttempts})...`);
            
            const health = await this.checkDatabaseHealth();
            if (health.status === 'healthy') {
                logger.success('Database is ready!');
                return true;
            }

            if (attempt < maxAttempts) {
                logger.warn(`Database not ready, retrying in ${delayMs}ms...`);
                await new Promise(resolve => setTimeout(resolve, delayMs));
            }
        }

        logger.error('Database failed to become ready after maximum attempts');
        return false;
    }
}

module.exports = HealthChecker;