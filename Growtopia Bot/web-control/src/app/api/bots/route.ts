import { NextRequest, NextResponse } from 'next/server'
import { prisma } from '@/lib/prisma'
import { getAuthUser } from '@/lib/auth'
import axios from 'axios'
import { Prisma } from '@prisma/client'

// GET - List all bots from user's nodes
export async function GET(request: NextRequest) {
  try {
    const user = await getAuthUser(request)
    if (!user) {
      return NextResponse.json({ error: 'Unauthorized' }, { status: 401 })
    }

    const where = user.role === 'ADMIN' ? {} : { node: { ownerId: user.id } }
    
    const bots = await prisma.bot.findMany({
      where,
      include: {
        node: {
          select: {
            id: true,
            name: true,
            apiUrl: true,
            isActive: true,
            owner: {
              select: {
                id: true,
                username: true,
                email: true
              }
            }
          }
        }
      },
      orderBy: {
        updatedAt: 'desc'
      }
    })

    return NextResponse.json(bots)
  } catch (error) {
    console.error('Get bots error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}

// POST - Sync bots from all active nodes
export async function POST(request: NextRequest) {
  try {
    const user = await getAuthUser(request)
    if (!user) {
      return NextResponse.json({ error: 'Unauthorized' }, { status: 401 })
    }

    const where = user.role === 'ADMIN' 
      ? { isActive: true }
      : { ownerId: user.id, isActive: true }

    const nodes = await prisma.node.findMany({ where })
    
    let syncedCount = 0
    const errors: string[] = []

    for (const node of nodes) {
      try {
        // Fetch bots from node API
        const response = await axios.get(`${node.apiUrl}/api/1.0/clients`, {
          timeout: 5000
        })
        
        const clients = response.data.clients || []
        
        // Node is active

        // Get current bots for this node from database
        const currentBots = await prisma.bot.findMany({
          where: { nodeId: node.id },
          select: { clientId: true }
        })
        
        const activeClientIds = clients.map((client: any) => client.id)
        
        // Remove bots that no longer exist on the node (cleanup from crashes)
        const botsToRemove = currentBots.filter(bot => !activeClientIds.includes(bot.clientId))
        if (botsToRemove.length > 0) {
          await prisma.bot.deleteMany({
            where: {
              clientId: { in: botsToRemove.map(bot => bot.clientId) },
              nodeId: node.id
            }
          })
        }
        
        // Sync bots
        for (const client of clients) {
          await prisma.bot.upsert({
            where: {
              clientId: client.id
            },
            update: {
              status: client.connected ? 'ONLINE' : 'OFFLINE',
              world: client.world || null,
              position: client.position ? `${client.position.x},${client.position.y}` : Prisma.JsonNull,
              inventory: client.inventory ? JSON.stringify(client.inventory) : Prisma.JsonNull,
              lastSeen: new Date(),
              updatedAt: new Date()
            },
            create: {
              clientId: client.id,
              name: client.username || `Bot ${client.id}`,
              username: client.username || `bot_${client.id}`,
              status: client.connected ? 'ONLINE' : 'OFFLINE',
              world: client.world || null,
              position: client.position ? `${client.position.x},${client.position.y}` : Prisma.JsonNull,
              inventory: client.inventory ? JSON.stringify(client.inventory) : Prisma.JsonNull,
              nodeId: node.id
            }
          })
        }
        
        syncedCount++
      } catch (error) {
        console.error(`Error syncing node ${node.name}:`, error)
        errors.push(`Node ${node.name}: ${error instanceof Error ? error.message : 'Unknown error'}`)
      }
    }

    return NextResponse.json({
      message: `Berhasil sync ${syncedCount} dari ${nodes.length} node`,
      syncedCount,
      totalNodes: nodes.length,
      errors
    })
  } catch (error) {
    console.error('Sync bots error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}