import { NextRequest, NextResponse } from 'next/server'
import { prisma } from '@/lib/prisma'
import { getAuthUser } from '@/lib/auth'

export async function GET(request: NextRequest) {
  try {
    const user = await getAuthUser(request)
    if (!user) {
      return NextResponse.json({ error: 'Unauthorized' }, { status: 401 })
    }

    const isAdmin = user.role === 'ADMIN'
    const userFilter = isAdmin ? {} : { ownerId: user.id }
    const botFilter = isAdmin ? {} : { node: { ownerId: user.id } }

    // Get basic stats
    const [totalNodes, activeNodes, totalBots, onlineBots, totalUsers] = await Promise.all([
      prisma.node.count({ where: userFilter }),
      prisma.node.count({ where: { ...userFilter, isActive: true } }),
      prisma.bot.count({ where: botFilter }),
      prisma.bot.count({ where: { ...botFilter, status: 'ONLINE' } }),
      isAdmin ? prisma.user.count() : Promise.resolve(null)
    ])

    // Get nodes by status
    const nodesByStatus = await prisma.node.groupBy({
      by: ['isActive'],
      where: userFilter,
      _count: {
        id: true
      }
    })

    // Get bots by status
    const botsByStatus = await prisma.bot.groupBy({
      by: ['status'],
      where: botFilter,
      _count: {
        id: true
      }
    })

    // Get recent activity (last 7 days)
    const sevenDaysAgo = new Date()
    sevenDaysAgo.setDate(sevenDaysAgo.getDate() - 7)

    const recentNodes = await prisma.node.findMany({
      where: {
        ...userFilter,
        createdAt: {
          gte: sevenDaysAgo
        }
      },
      select: {
        createdAt: true
      }
    })

    // Group by day
    const activityByDay = recentNodes.reduce((acc: Record<string, number>, node: { createdAt: Date }) => {
      const day = node.createdAt.toISOString().split('T')[0]
      acc[day] = (acc[day] || 0) + 1
      return acc
    }, {} as Record<string, number>)

    // Fill missing days with 0
    const last7Days = []
    for (let i = 6; i >= 0; i--) {
      const date = new Date()
      date.setDate(date.getDate() - i)
      const dayStr = date.toISOString().split('T')[0]
      last7Days.push({
        date: dayStr,
        count: activityByDay[dayStr] || 0
      })
    }

    // Get top performing nodes (by bot count)
    const topNodes = await prisma.node.findMany({
      where: userFilter,
      include: {
        _count: {
          select: {
            bots: true
          }
        },
        owner: {
          select: {
            username: true
          }
        }
      },
      orderBy: {
        bots: {
          _count: 'desc'
        }
      },
      take: 5
    })

    const stats = {
      overview: {
        totalNodes,
        activeNodes,
        totalBots,
        onlineBots,
        ...(isAdmin && { totalUsers })
      },
      nodesByStatus: nodesByStatus.map((item) => ({
        status: item.isActive ? 'Active' : 'Inactive',
        count: item._count.id
      })),
      botsByStatus: botsByStatus.map((item: { status: string, _count: { id: number } }) => ({
        status: item.status,
        count: item._count.id
      })),
      activityChart: last7Days,
      topNodes: topNodes.map((node: { id: string, name: string, _count: { bots: number }, isActive: boolean, owner: { username: string } }) => ({
        id: node.id,
        name: node.name,
        botCount: node._count.bots,
        isActive: node.isActive,
        owner: node.owner.username
      }))
    }

    return NextResponse.json(stats)
  } catch (error) {
    console.error('Dashboard stats error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}