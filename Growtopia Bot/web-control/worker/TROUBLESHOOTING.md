# Growtopia Bot Worker - Troubleshooting Guide

## Common Issues and Solutions

### 1. Database Connection Issues

#### Error: `P2021 - Database table does not exist`
**Cause:** Database hasn't been initialized or migrations haven't been run.

**Solution:**
```bash
# Run database migration
npx prisma migrate dev --name init

# Generate Prisma client
npx prisma generate
```

#### Error: `EPERM: operation not permitted`
**Cause:** File permission issues with Prisma client generation.

**Solution:**
1. Close any running Node.js processes
2. Delete `node_modules/.prisma` folder
3. Run `npm install` again
4. Try `npx prisma generate` again

#### Error: `Database connection failed`
**Cause:** Invalid DATABASE_URL or database file doesn't exist.

**Solution:**
1. Check `.env` file for correct DATABASE_URL
2. Ensure the database file path is accessible
3. Run migrations to create the database

### 2. Node Connection Issues

#### Warning: `Node X is not responding`
**Cause:** Target node server is down or unreachable.

**Solution:**
1. Verify the node server is running
2. Check host and port configuration
3. Test connectivity: `curl http://host:port/health`
4. Check firewall settings

#### Error: `ECONNREFUSED`
**Cause:** Connection refused by target server.

**Solution:**
1. Ensure the target server is running
2. Verify the correct port is being used
3. Check if the service is bound to the correct interface

#### Error: `ETIMEDOUT`
**Cause:** Request timeout.

**Solution:**
1. Increase timeout values in config.js
2. Check network connectivity
3. Verify server response time

### 3. Worker Process Issues

#### Error: `Sync already in progress`
**Cause:** Previous sync operation is still running.

**Solution:**
- This is normal behavior - the worker prevents overlapping syncs
- If sync seems stuck, restart the worker
- Check logs for any errors in the sync process

#### Error: `CRITICAL: X consecutive sync failures`
**Cause:** Multiple sync operations have failed.

**Solution:**
1. Check database connectivity
2. Verify node configurations
3. Review error logs for specific issues
4. Consider restarting the worker

### 4. Performance Issues

#### Slow sync operations
**Causes and Solutions:**
1. **Large number of bots:** Increase timeout values
2. **Slow database:** Consider using PostgreSQL instead of SQLite
3. **Network latency:** Reduce sync frequency
4. **Resource constraints:** Monitor CPU and memory usage

#### High memory usage
**Solutions:**
1. Restart worker periodically
2. Reduce sync frequency
3. Optimize database queries
4. Monitor for memory leaks

## Diagnostic Commands

### Check Worker Status
```bash
# View worker logs
tail -f worker.log

# Check if worker is running
ps aux | grep node

# Monitor resource usage
top -p $(pgrep -f "node index.js")
```

### Database Diagnostics
```bash
# Check database schema
npx prisma db pull

# View database contents
npx prisma studio

# Reset database (WARNING: deletes all data)
npx prisma migrate reset --force

# Test database connection
node test-data.js stats
```

### Network Diagnostics
```bash
# Test node connectivity
curl -v http://localhost:8080/health

# Check port availability
netstat -an | grep :8080

# Test DNS resolution
nslookup your-node-hostname
```

## Configuration Tuning

### For High-Volume Environments
```env
# Increase timeouts
NODE_TIMEOUT_MS=10000
BOT_API_TIMEOUT_MS=15000

# Reduce sync frequency
SYNC_INTERVAL_MINUTES=10

# Enable debug logging
LOG_LEVEL=debug
```

### For Low-Resource Environments
```env
# Reduce timeouts
NODE_TIMEOUT_MS=3000
BOT_API_TIMEOUT_MS=5000

# Increase sync frequency
SYNC_INTERVAL_MINUTES=2

# Reduce logging
LOG_LEVEL=warn
```

## Log Analysis

### Important Log Patterns

#### Successful Operation
```
[SUCCESS] Database connected successfully
[INFO] Found X nodes in database
[SUCCESS] Sync completed: X/Y nodes active
[SUCCESS] Worker started successfully
```

#### Warning Signs
```
[WARNING] Node X is not responding
[WARNING] Filtered out X invalid bots
[ERROR] Failed to connect to database
[CRITICAL] X consecutive sync failures
```

#### Critical Issues
```
[CRITICAL] Uncaught Exception detected
[CRITICAL] Database health check failed
[FATAL] Failed to initialize worker
```

## Recovery Procedures

### Complete Reset
```bash
# Stop worker
Ctrl+C or ./stop.bat

# Reset database
npx prisma migrate reset --force

# Clear logs
rm worker.log

# Restart worker
node index.js
```

### Partial Recovery
```bash
# Clear test data
node test-data.js clear

# Add fresh test data
node test-data.js add

# Check status
node test-data.js stats
```

## Getting Help

### Before Reporting Issues
1. Check this troubleshooting guide
2. Review recent log entries
3. Test with minimal configuration
4. Verify system requirements

### Information to Include
1. Error messages (full stack trace)
2. Configuration files (.env, config.js)
3. System information (OS, Node.js version)
4. Steps to reproduce the issue
5. Recent log entries

### Useful Commands for Bug Reports
```bash
# System information
node --version
npm --version
uname -a  # Linux/Mac
systeminfo  # Windows

# Worker information
node -e "console.log(require('./package.json').version)"
node test-data.js stats

# Recent logs
tail -50 worker.log
```

## Monitoring and Maintenance

### Regular Maintenance Tasks
1. **Daily:** Check worker logs for errors
2. **Weekly:** Review sync statistics
3. **Monthly:** Clean up old log files
4. **Quarterly:** Update dependencies

### Monitoring Checklist
- [ ] Worker process is running
- [ ] Database is accessible
- [ ] Sync operations completing successfully
- [ ] No critical errors in logs
- [ ] Reasonable resource usage
- [ ] All expected nodes are responding

### Automated Monitoring
Consider implementing:
1. Process monitoring (PM2, systemd)
2. Log rotation (logrotate)
3. Health check endpoints
4. Alerting for critical errors
5. Performance metrics collection