module.exports = {
  // Sync configuration
  sync: {
    intervalMinutes: process.env.SYNC_INTERVAL_MINUTES || 5,
    nodeTimeoutMs: process.env.NODE_TIMEOUT_MS || 5000,
    botApiTimeoutMs: process.env.BOT_API_TIMEOUT_MS || 10000,
    maxRetries: 3,
    retryDelayMs: 1000
  },

  // Database configuration
  database: {
    url: process.env.DATABASE_URL || 'file:../prisma/dev.db'
  },

  // Logging configuration
  logging: {
    level: process.env.LOG_LEVEL || 'info',
    file: process.env.LOG_FILE || 'worker.log',
    console: true,
    timestamp: true
  },

  // Node API endpoints
  endpoints: {
    health: '/health',
    bots: '/api/bots'
  },

  // Bot status mapping
  botStatus: {
    active: ['ONLINE', 'CONNECTED', 'ONWORLD'],
    inactive: ['OFFLINE', 'DISCONNECTED', 'SUSPENDED']
  }
};