const fs = require('fs');
const path = require('path');
const config = require('./config');

class Logger {
  constructor() {
    this.logFile = config.logging.file;
    this.logLevel = config.logging.level;
    this.enableConsole = config.logging.console;
    this.enableTimestamp = config.logging.timestamp;
    
    // Ensure log directory exists
    const logDir = path.dirname(this.logFile);
    if (!fs.existsSync(logDir)) {
      fs.mkdirSync(logDir, { recursive: true });
    }
  }

  formatMessage(level, message, data = null) {
    const timestamp = this.enableTimestamp ? new Date().toISOString() : '';
    const prefix = timestamp ? `[${timestamp}] ` : '';
    const dataStr = data ? ` ${JSON.stringify(data)}` : '';
    return `${prefix}[${level.toUpperCase()}] ${message}${dataStr}`;
  }

  writeToFile(message) {
    try {
      fs.appendFileSync(this.logFile, message + '\n');
    } catch (error) {
      console.error('Failed to write to log file:', error);
    }
  }

  log(level, message, data = null) {
    const formattedMessage = this.formatMessage(level, message, data);
    
    if (this.enableConsole) {
      console.log(formattedMessage);
    }
    
    this.writeToFile(formattedMessage);
  }

  info(message, data = null) {
    this.log('info', message, data);
  }

  warn(message, data = null) {
    this.log('warn', message, data);
  }

  error(message, data = null) {
    this.log('error', message, data);
  }

  debug(message, data = null) {
    if (this.logLevel === 'debug') {
      this.log('debug', message, data);
    }
  }

  success(message, data = null) {
    this.log('success', message, data);
  }
}

module.exports = new Logger();