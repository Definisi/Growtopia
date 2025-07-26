import { NextRequest, NextResponse } from 'next/server'
import { prisma } from '@/lib/prisma'
import { getAuthUser } from '@/lib/auth'
import axios from 'axios'

// POST - Send action to bot
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

    const { action, data } = await request.json()

    if (!action) {
      return NextResponse.json(
        { error: 'Action harus diisi' },
        { status: 400 }
      )
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

    // Prepare action payload based on action type
    let actionPayload: any = { action }

    switch (action) {
      case 'punch':
        actionPayload = {
          action: 'punch',
          direction: data?.direction || 'left' // left, right, up, down
        }
        break
      case 'move':
        actionPayload = {
          action: 'move',
          x: data?.x || 0,
          y: data?.y || 0
        }
        break
      case 'place':
        actionPayload = {
          action: 'place',
          itemId: data?.itemId,
          x: data?.x || 0,
          y: data?.y || 0
        }
        break
      case 'collect':
        actionPayload = {
          action: 'collect',
          x: data?.x || 0,
          y: data?.y || 0
        }
        break
      case 'warp':
        actionPayload = {
          action: 'warp',
          world: data?.world
        }
        break
      case 'say':
        actionPayload = {
          action: 'say',
          message: data?.message
        }
        break
      case 'disconnect':
        actionPayload = {
          action: 'disconnect'
        }
        break
      case 'reconnect':
        actionPayload = {
          action: 'reconnect'
        }
        break
      default:
        return NextResponse.json(
          { error: 'Action tidak valid' },
          { status: 400 }
        )
    }

    try {
      // Send action to node API
      const response = await axios.post(
        `${bot.node.apiUrl}/api/1.0/clients/${bot.username}/action`,
        actionPayload,
        {
          timeout: 10000,
          headers: {
            'Content-Type': 'application/json'
          }
        }
      )

      // Update bot status if needed
      if (action === 'disconnect') {
        await prisma.bot.update({
          where: { id: bot.id },
          data: { status: 'OFFLINE' }
        })
      } else if (action === 'reconnect') {
        await prisma.bot.update({
          where: { id: bot.id },
          data: { status: 'CONNECTING' }
        })
      }

      return NextResponse.json({
        message: `Action ${action} berhasil dikirim ke bot ${bot.username}`,
        response: response.data
      })
    } catch (apiError) {
      console.error('Node API error:', apiError)
      return NextResponse.json(
        { error: 'Gagal mengirim action ke node' },
        { status: 502 }
      )
    }
  } catch (error) {
    console.error('Bot action error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}