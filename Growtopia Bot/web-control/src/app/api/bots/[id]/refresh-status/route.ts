import { NextRequest, NextResponse } from 'next/server'
import { prisma } from '@/lib/prisma'
import { getAuthUser } from '@/lib/auth'
import axios from 'axios'

// POST - Refresh bot status
export async function POST(
  request: NextRequest,
  { params }: { params: Promise<{ id: string }> }
) {
  const { id } = await params
  try {
    const user = await getAuthUser(request)
    if (!user) {
      return NextResponse.json({ error: 'Unauthorized' }, { status: 401 })
    }

    // Find bot and check ownership
    const where = user.role === 'ADMIN' 
      ? { id: id }
      : { id: id, node: { ownerId: user.id } }

    const bot = await prisma.bot.findFirst({
      where,
      include: {
        node: true
      }
    })

    if (!bot) {
      return NextResponse.json({ error: 'Bot tidak ditemukan' }, { status: 404 })
    }

    if (!bot.node.isActive) {
      return NextResponse.json({ error: 'Node tidak aktif' }, { status: 400 })
    }

    // Get bot status from node
    let botStatus: 'ONLINE' | 'OFFLINE' | 'CONNECTING' | 'ERROR' = 'OFFLINE'
    try {
      // Call node API to check bot status
      const response = await axios.post(`${bot.node.apiUrl}/api/1.0`, {
        option: 'status',
        tank_id_name: bot.clientId
      }, {
        timeout: 5000
      })
      
      // Update status based on response
      if (response.data.success && response.data.connected) {
        botStatus = 'ONLINE'
      }
    } catch (statusError) {
      console.warn(`Couldn't get status for bot ${bot.clientId}:`, statusError)
      // Keep OFFLINE status if failed to get status
    }

    // Update bot status in database
    const updatedBot = await prisma.bot.update({
      where: { id: bot.id },
      data: { status: botStatus }
    })

    return NextResponse.json({
      message: 'Status bot berhasil diperbarui',
      status: botStatus,
      bot: updatedBot
    })
  } catch (error: any) {
    console.error('Error refreshing bot status:', error)
    return NextResponse.json(
      { error: 'Gagal memperbarui status bot', details: error.message },
      { status: 500 }
    )
  }
}