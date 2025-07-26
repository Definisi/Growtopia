require('dotenv').config();
const { PrismaClient } = require('@prisma/client');
const logger = require('./logger');

const prisma = new PrismaClient();

async function addTestData() {
    try {
        logger.info('Adding test data to database...');
        
        // Add test nodes
        const node1 = await prisma.node.create({
            data: {
                name: 'Test Node 1',
                host: 'localhost',
                port: 8080,
                status: 'online'
            }
        });
        
        const node2 = await prisma.node.create({
            data: {
                name: 'Test Node 2',
                host: 'localhost',
                port: 8081,
                status: 'offline'
            }
        });
        
        logger.success(`Created nodes: ${node1.name} (ID: ${node1.id}), ${node2.name} (ID: ${node2.id})`);
        
        // Add test bots
        const bot1 = await prisma.bot.create({
            data: {
                name: 'TestBot1',
                username: 'testuser1',
                password: 'testpass1',
                status: 'online',
                nodeId: node1.id
            }
        });
        
        const bot2 = await prisma.bot.create({
            data: {
                name: 'TestBot2',
                username: 'testuser2',
                password: 'testpass2',
                status: 'offline',
                nodeId: node1.id
            }
        });
        
        const bot3 = await prisma.bot.create({
            data: {
                name: 'TestBot3',
                username: 'testuser3',
                password: 'testpass3',
                status: 'online',
                nodeId: node2.id
            }
        });
        
        logger.success(`Created bots: ${bot1.name} (ID: ${bot1.id}), ${bot2.name} (ID: ${bot2.id}), ${bot3.name} (ID: ${bot3.id})`);
        
        // Display statistics
        const stats = {
            totalNodes: await prisma.node.count(),
            activeNodes: await prisma.node.count({ where: { status: 'online' } }),
            totalBots: await prisma.bot.count(),
            activeBots: await prisma.bot.count({ where: { status: 'online' } })
        };
        
        logger.info('Database statistics:');
        logger.info(`  Nodes: ${stats.activeNodes}/${stats.totalNodes} active`);
        logger.info(`  Bots: ${stats.activeBots}/${stats.totalBots} active`);
        
    } catch (error) {
        logger.error('Error adding test data:', error.message);
    } finally {
        await prisma.$disconnect();
    }
}

async function clearTestData() {
    try {
        logger.info('Clearing test data from database...');
        
        // Delete all bots first (due to foreign key constraints)
        const deletedBots = await prisma.bot.deleteMany({});
        logger.info(`Deleted ${deletedBots.count} bots`);
        
        // Delete all nodes
        const deletedNodes = await prisma.node.deleteMany({});
        logger.info(`Deleted ${deletedNodes.count} nodes`);
        
        logger.success('Test data cleared successfully');
        
    } catch (error) {
        logger.error('Error clearing test data:', error.message);
    } finally {
        await prisma.$disconnect();
    }
}

async function showStats() {
    try {
        const stats = {
            totalNodes: await prisma.node.count(),
            activeNodes: await prisma.node.count({ where: { status: 'online' } }),
            totalBots: await prisma.bot.count(),
            activeBots: await prisma.bot.count({ where: { status: 'online' } })
        };
        
        logger.info('Current database statistics:');
        logger.info(`  Nodes: ${stats.activeNodes}/${stats.totalNodes} active`);
        logger.info(`  Bots: ${stats.activeBots}/${stats.totalBots} active`);
        
        // Show detailed data
        const nodes = await prisma.node.findMany({
            include: {
                bots: true
            }
        });
        
        logger.info('\nDetailed data:');
        for (const node of nodes) {
            logger.info(`  Node: ${node.name} (${node.status}) - ${node.bots.length} bots`);
            for (const bot of node.bots) {
                logger.info(`    Bot: ${bot.name || bot.username} (${bot.status})`);
            }
        }
        
    } catch (error) {
        logger.error('Error getting stats:', error.message);
    } finally {
        await prisma.$disconnect();
    }
}

// Command line interface
const command = process.argv[2];

switch (command) {
    case 'add':
        addTestData();
        break;
    case 'clear':
        clearTestData();
        break;
    case 'stats':
        showStats();
        break;
    default:
        console.log('Usage:');
        console.log('  node test-data.js add    - Add test data');
        console.log('  node test-data.js clear  - Clear all data');
        console.log('  node test-data.js stats  - Show statistics');
        process.exit(1);
}