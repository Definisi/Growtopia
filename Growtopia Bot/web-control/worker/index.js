require('dotenv').config();
const cron = require('node-cron');
const { PrismaClient } = require('@prisma/client');
const axios = require('axios');
const config = require('./config');
const logger = require('./logger');
const ErrorHandler = require('./error-handler');
const HealthChecker = require('./health-check');

const prisma = new PrismaClient();

class BotSyncWorker {
  constructor() {
    this.healthChecker = new HealthChecker();
    this.isRunning = false;
    this.syncInProgress = false;
    this.consecutiveFailures = 0;
    this.maxConsecutiveFailures = 5;
  }

  async syncBots() {
    if (this.syncInProgress) {
      logger.warn('Sync already in progress, skipping...');
      return;
    }

    this.syncInProgress = true;
    logger.info('Starting bot synchronization...');

    try {
      // Check database health first
      const dbHealth = await this.healthChecker.checkDatabaseHealth();
      if (dbHealth.status !== 'healthy') {
        throw new Error(`Database health check failed: ${dbHealth.message}`);
      }

      // Get all nodes from database
      const nodes = await prisma.node.findMany({
        include: {
          bots: true
        }
      });

      logger.info(`Found ${nodes.length} nodes in database`);

      let errors = [];
      let activeNodes = 0;

      for (const node of nodes) {
        try {
          const isActive = await this.syncNodeBots(node);
          if (isActive) {
            activeNodes++;
          }
        } catch (nodeError) {
          const processedError = ErrorHandler.processError(nodeError, `Node: ${node.name}`);
          errors.push(`${node.name}: ${processedError.message}`);
        }
      }

      // Get final statistics
      const totalBots = await prisma.bot.count();
      const activeBots = await prisma.bot.count({ where: { status: { in: ['ONLINE', 'CONNECTED'] } } });
      
      logger.success(`Sync completed: ${activeNodes}/${nodes.length} nodes active, ${totalBots} total bots (${activeBots} active)`);
      
      if (errors.length > 0) {
        logger.warn(`Encountered ${errors.length} errors during sync:`);
        errors.forEach(error => logger.warn(`  - ${error}`));
      }

      // Reset consecutive failures on successful sync
      this.consecutiveFailures = 0;

    } catch (error) {
      this.consecutiveFailures++;
      const processedError = ErrorHandler.processError(error, 'Bot synchronization');
      
      if (this.consecutiveFailures >= this.maxConsecutiveFailures) {
        logger.error(`CRITICAL: ${this.consecutiveFailures} consecutive sync failures. Worker may need manual intervention.`);
      }
      
      // Re-throw critical errors
      if (processedError.severity === 'critical') {
        throw error;
      }
    } finally {
      this.syncInProgress = false;
    }
  }

  async syncNodeBots(node) {
    logger.info(`Syncing node: ${node.name} (${node.host}:${node.port})`);

    try {
      // Check if node is active by making a health check request
      const isNodeActive = await this.checkNodeHealth(node);

      if (!isNodeActive) {
        logger.warn(`Node ${node.name} is inactive, removing all bots...`);
        await this.removeAllBotsFromNode(node.id);
        return;
      }

      logger.info(`Node ${node.name} is active, checking bots...`);

      // Get active bots from the node
      const activeBots = await this.getActiveBotsFromNode(node);
      logger.info(`Found ${activeBots.length} active bots on node`);

      // Get bots from database for this node
      const dbBots = node.bots;
      logger.info(`Found ${dbBots.length} bots in database for this node`);

      // Remove bots from database that are not active on the node
      await this.removeInactiveBots(dbBots, activeBots, node.id);

      // Add/update active bots in database
      await this.updateActiveBots(activeBots, node.id);

    } catch (error) {
      logger.error(`Error syncing node ${node.name}:`, error.message);
    }
  }

  async checkNodeHealth(node) {
    try {
      const response = await axios.get(`http://${node.host}:${node.port}/health`, {
        timeout: 5000
      });
      return response.status === 200;
    } catch (error) {
      logger.debug(`Node ${node.name} health check failed: ${error.message}`);
      return false;
    }
  }

  async getActiveBotsFromNode(node) {
    try {
      const response = await axios.get(`http://${node.host}:${node.port}/api/bots`, {
        timeout: 10000
      });
      
      if (response.data && response.data.bots) {
        return response.data.bots.filter(bot => bot.status === 'ONLINE' || bot.status === 'CONNECTED');
      }
      return [];
    } catch (error) {
      logger.error(`Failed to get bots from node ${node.name}:`, error.message);
      return [];
    }
  }

  async removeAllBotsFromNode(nodeId) {
    try {
      const result = await prisma.bot.deleteMany({
        where: {
          nodeId: nodeId
        }
      });
      logger.info(`Removed ${result.count} bots from inactive node`);
    } catch (error) {
      logger.error('Error removing bots from inactive node:', error);
    }
  }

  async removeInactiveBots(dbBots, activeBots, nodeId) {
    const activeUsernames = new Set(activeBots.map(bot => bot.username || bot.tankIdName));
    
    for (const dbBot of dbBots) {
      if (!activeUsernames.has(dbBot.username)) {
        try {
          await prisma.bot.delete({
            where: {
              id: dbBot.id
            }
          });
          logger.info(`Removed inactive bot: ${dbBot.username}`);
        } catch (error) {
          logger.error(`Error removing bot ${dbBot.username}:`, error);
        }
      }
    }
  }

  async updateActiveBots(activeBots, nodeId) {
    for (const activeBot of activeBots) {
      const username = activeBot.username || activeBot.tankIdName;
      if (!username) continue;

      try {
        // Check if bot exists in database
        const existingBot = await prisma.bot.findFirst({
          where: {
            username: username,
            nodeId: nodeId
          }
        });

        if (existingBot) {
          // Update existing bot
          await prisma.bot.update({
            where: {
              id: existingBot.id
            },
            data: {
              status: activeBot.status || 'ONLINE',
              lastSeen: new Date(),
              // Add other fields as needed
            }
          });
          logger.debug(`Updated bot: ${username}`);
        } else {
          // Create new bot
          await prisma.bot.create({
            data: {
              username: username,
              password: '', // We don't have password from node API
              status: activeBot.status || 'ONLINE',
              nodeId: nodeId,
              lastSeen: new Date(),
              // Add other fields as needed
            }
          });
          logger.info(`Added new bot: ${username}`);
        }
      } catch (error) {
        logger.error(`Error updating bot ${username}:`, error);
      }
    }
  }

  async start() {
    logger.info('Starting Growtopia Bot Worker...');
    
    try {
      // Wait for database to be ready
      logger.info('Checking database readiness...');
      const dbReady = await this.healthChecker.waitForDatabaseReady(10, 3000);
      if (!dbReady) {
        throw new Error('Database failed to become ready within timeout period');
      }

      // Perform initial health check
      const healthCheck = await this.healthChecker.performFullHealthCheck();
      if (!healthCheck.summary.healthy) {
        logger.warn('System health issues detected, but continuing startup...');
        logger.warn('Issues:', healthCheck.summary.issues.join(', '));
      }

      // Schedule sync job
      const cronExpression = `*/${config.sync.intervalMinutes} * * * *`;
      logger.info(`Scheduling sync every ${config.sync.intervalMinutes} minutes`);
      
      cron.schedule(cronExpression, async () => {
        if (!this.syncInProgress) {
          await this.syncBots();
        } else {
          logger.debug('Skipping scheduled sync - previous sync still in progress');
        }
      });

      // Run initial sync with error handling
      logger.info('Running initial sync...');
      try {
        await this.syncBots();
      } catch (error) {
        logger.warn('Initial sync failed, but worker will continue with scheduled syncs');
        ErrorHandler.processError(error, 'Initial sync');
      }

      this.isRunning = true;
      logger.success('Worker started successfully');
      
      // Log system information
      const stats = await this.getStats();
      logger.info(`System status: ${stats.nodes.active}/${stats.nodes.total} nodes active, ${stats.bots.active}/${stats.bots.total} bots active`);
      
    } catch (error) {
      const processedError = ErrorHandler.processError(error, 'Worker startup');
      logger.error('Failed to start worker:', processedError.message);
      logger.error('Solution:', processedError.solution);
      process.exit(1);
    }
  }

  async getStats() {
     try {
       const totalNodes = await prisma.node.count();
       const activeNodes = await prisma.node.count({ where: { status: 'online' } });
       const totalBots = await prisma.bot.count();
       const activeBots = await prisma.bot.count({ where: { status: 'online' } });

       return {
         nodes: { total: totalNodes, active: activeNodes },
         bots: { total: totalBots, active: activeBots }
       };
     } catch (error) {
       logger.error('Error getting stats:', error);
       return {
         nodes: { total: 0, active: 0 },
         bots: { total: 0, active: 0 }
       };
     }
   }

  async stop() {
    logger.info('Stopping Bot Sync Worker...');
    this.isRunning = false;
    
    // Wait for any ongoing sync to complete
    let waitCount = 0;
    while (this.syncInProgress && waitCount < 30) {
      logger.info('Waiting for ongoing sync to complete...');
      await new Promise(resolve => setTimeout(resolve, 1000));
      waitCount++;
    }
    
    if (this.syncInProgress) {
      logger.warn('Forced shutdown - sync operation may have been interrupted');
    }
    
    // Disconnect from database
    try {
      await prisma.$disconnect();
      logger.info('Database disconnected');
    } catch (error) {
      logger.warn('Error disconnecting from database:', error.message);
    }
    
    logger.success('Worker stopped gracefully');
    process.exit(0);
  }

  async getHealthStatus() {
    return await this.healthChecker.performFullHealthCheck();
  }

  getWorkerStatus() {
    return {
      isRunning: this.isRunning,
      syncInProgress: this.syncInProgress,
      consecutiveFailures: this.consecutiveFailures,
      maxConsecutiveFailures: this.maxConsecutiveFailures,
      uptime: process.uptime(),
      memoryUsage: process.memoryUsage(),
      version: require('./package.json').version || '1.0.0'
    };
  }
}

// Start the worker
const worker = new BotSyncWorker();

// Enhanced graceful shutdown handling
let shutdownInProgress = false;

const gracefulShutdown = async (signal) => {
  if (shutdownInProgress) {
    logger.warn('Shutdown already in progress, forcing exit...');
    process.exit(1);
  }
  
  shutdownInProgress = true;
  logger.info(`Received ${signal}, shutting down gracefully...`);
  
  try {
    await worker.stop();
  } catch (error) {
    logger.error('Error during graceful shutdown:', error.message);
    process.exit(1);
  }
};

process.on('SIGINT', () => gracefulShutdown('SIGINT'));
process.on('SIGTERM', () => gracefulShutdown('SIGTERM'));

// Enhanced error handling
process.on('uncaughtException', (error) => {
  const processedError = ErrorHandler.processError(error, 'Uncaught Exception');
  logger.error('CRITICAL: Uncaught Exception detected');
  logger.error('This indicates a serious bug that needs immediate attention');
  
  // Try graceful shutdown
  gracefulShutdown('UNCAUGHT_EXCEPTION').catch(() => {
    process.exit(1);
  });
});

process.on('unhandledRejection', (reason, promise) => {
  const error = reason instanceof Error ? reason : new Error(String(reason));
  const processedError = ErrorHandler.processError(error, 'Unhandled Promise Rejection');
  
  logger.error('CRITICAL: Unhandled Promise Rejection detected');
  logger.error('Promise:', promise);
  
  // Try graceful shutdown
  gracefulShutdown('UNHANDLED_REJECTION').catch(() => {
    process.exit(1);
  });
});

// Start the worker with enhanced error handling
(async () => {
  try {
    await worker.start();
    logger.success('Growtopia Bot Worker initialized successfully');
    
    // Log worker status periodically
    setInterval(() => {
      const status = worker.getWorkerStatus();
      logger.debug(`Worker status: Running=${status.isRunning}, Sync=${status.syncInProgress}, Failures=${status.consecutiveFailures}`);
    }, 300000); // Every 5 minutes
    
  } catch (error) {
    const processedError = ErrorHandler.processError(error, 'Worker initialization');
    logger.error('FATAL: Failed to initialize worker');
    logger.error('The application cannot continue and will exit');
    process.exit(1);
  }
})();

logger.info('Bot Sync Worker is running...');
logger.info('Press Ctrl+C to stop');