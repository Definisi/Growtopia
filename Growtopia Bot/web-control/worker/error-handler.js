const logger = require('./logger');

class ErrorHandler {
    static handlePrismaError(error) {
        const errorMap = {
            'P2021': {
                message: 'Database table does not exist. Run migrations first.',
                solution: 'Execute: npx prisma migrate dev --name init',
                severity: 'critical'
            },
            'P2002': {
                message: 'Unique constraint violation.',
                solution: 'Check for duplicate data before insertion.',
                severity: 'warning'
            },
            'P2025': {
                message: 'Record not found.',
                solution: 'Verify the record exists before operation.',
                severity: 'info'
            },
            'P1001': {
                message: 'Cannot reach database server.',
                solution: 'Check database connection and server status.',
                severity: 'critical'
            },
            'P1003': {
                message: 'Database does not exist.',
                solution: 'Create database or check DATABASE_URL.',
                severity: 'critical'
            }
        };

        const errorInfo = errorMap[error.code] || {
            message: `Unknown Prisma error: ${error.message}`,
            solution: 'Check Prisma documentation for error code: ' + error.code,
            severity: 'error'
        };

        return {
            type: 'PrismaError',
            code: error.code,
            ...errorInfo,
            originalMessage: error.message,
            meta: error.meta || {}
        };
    }

    static handleNetworkError(error, context = '') {
        const errorMap = {
            'ECONNREFUSED': {
                message: 'Connection refused by target server.',
                solution: 'Check if the server is running and accessible.',
                severity: 'warning'
            },
            'ETIMEDOUT': {
                message: 'Request timed out.',
                solution: 'Check network connectivity or increase timeout.',
                severity: 'warning'
            },
            'ENOTFOUND': {
                message: 'Host not found.',
                solution: 'Verify the hostname/IP address is correct.',
                severity: 'error'
            },
            'ECONNRESET': {
                message: 'Connection reset by peer.',
                solution: 'Server may be overloaded or restarting.',
                severity: 'warning'
            }
        };

        const errorInfo = errorMap[error.code] || {
            message: `Network error: ${error.message}`,
            solution: 'Check network connectivity and server status.',
            severity: 'error'
        };

        return {
            type: 'NetworkError',
            code: error.code,
            context,
            ...errorInfo,
            originalMessage: error.message
        };
    }

    static handleGenericError(error, context = '') {
        return {
            type: 'GenericError',
            context,
            message: error.message || 'Unknown error occurred',
            solution: 'Check logs for more details and contact support if needed.',
            severity: 'error',
            stack: error.stack
        };
    }

    static processError(error, context = '') {
        let processedError;

        if (error.name === 'PrismaClientKnownRequestError') {
            processedError = this.handlePrismaError(error);
        } else if (error.code && ['ECONNREFUSED', 'ETIMEDOUT', 'ENOTFOUND', 'ECONNRESET'].includes(error.code)) {
            processedError = this.handleNetworkError(error, context);
        } else {
            processedError = this.handleGenericError(error, context);
        }

        // Log based on severity
        switch (processedError.severity) {
            case 'critical':
                logger.error(`[CRITICAL] ${processedError.message}`);
                logger.error(`Solution: ${processedError.solution}`);
                break;
            case 'error':
                logger.error(`[ERROR] ${processedError.message}`);
                logger.info(`Solution: ${processedError.solution}`);
                break;
            case 'warning':
                logger.warn(`[WARNING] ${processedError.message}`);
                logger.debug(`Solution: ${processedError.solution}`);
                break;
            case 'info':
                logger.info(`[INFO] ${processedError.message}`);
                break;
        }

        return processedError;
    }

    static async withRetry(operation, maxRetries = 3, delayMs = 1000, context = '') {
        for (let attempt = 1; attempt <= maxRetries; attempt++) {
            try {
                return await operation();
            } catch (error) {
                const processedError = this.processError(error, context);
                
                if (attempt === maxRetries || processedError.severity === 'critical') {
                    throw error;
                }

                logger.warn(`Attempt ${attempt}/${maxRetries} failed, retrying in ${delayMs}ms...`);
                await new Promise(resolve => setTimeout(resolve, delayMs));
                delayMs *= 1.5; // Exponential backoff
            }
        }
    }

    static createSafeWrapper(fn, context = '') {
        return async (...args) => {
            try {
                return await fn(...args);
            } catch (error) {
                this.processError(error, context);
                return null; // or appropriate default value
            }
        };
    }
}

module.exports = ErrorHandler;