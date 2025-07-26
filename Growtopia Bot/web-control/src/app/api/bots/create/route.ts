import { NextRequest, NextResponse } from 'next/server'
import { prisma } from '@/lib/prisma'
import { getAuthUser } from '@/lib/auth'
import axios from 'axios'

// POST - Create a new bot
export async function POST(request: NextRequest) {
  try {
    const user = await getAuthUser(request)
    if (!user) {
      return NextResponse.json({ error: 'Unauthorized' }, { status: 401 })
    }

    const { name, nodeId, username, password } = await request.json()

    // Validate required fields
    if (!name || !nodeId || !username || !password) {
      return NextResponse.json(
        { error: 'Nama, node, username, dan password harus diisi' },
        { status: 400 }
      )
    }

    // Check if node exists and user has access
    const where = user.role === 'ADMIN' 
      ? { id: nodeId, isActive: true }
      : { id: nodeId, ownerId: user.id, isActive: true }

    const node = await prisma.node.findFirst({ where })
    
    if (!node) {
      return NextResponse.json(
        { error: 'Node tidak ditemukan atau tidak aktif' },
        { status: 404 }
      )
    }

    try {
      // Call node API to create bot
      const response = await axios.post(`${node.apiUrl}/api/1.0`, {
        option: 'add',
        tank_id_name: username,
        tank_id_pass: password,
        server: 'growtopia1.com',
        port: 17091
      }, {
        timeout: 30000 // Meningkatkan timeout menjadi 30 detik
      })

      if (!response.data.success) {
        return NextResponse.json(
          { error: response.data.error || 'Gagal membuat bot di node' },
          { status: 400 }
        )
      }

      // Cek status koneksi bot
      let botStatus: 'ONLINE' | 'OFFLINE' | 'CONNECTING' | 'ERROR' = 'OFFLINE'
      try {
        // Coba dapatkan status bot yang baru dibuat
        const statusResponse = await axios.post(`${node.apiUrl}/api/1.0`, {
          option: 'status',
          tank_id_name: username
        }, {
          timeout: 5000
        })
        
        // Update status berdasarkan respons
        if (statusResponse.data.success && statusResponse.data.connected) {
          botStatus = 'ONLINE'
        }
      } catch (statusError) {
        console.warn(`Couldn't get status for new bot ${username}:`, statusError)
        // Tetap gunakan status OFFLINE jika gagal mendapatkan status
      }
      
      // Generate unique clientId to avoid conflicts
      const clientId = response.data.client_id || `${username}_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`
      
      // Check if bot with same clientId already exists and delete it (cleanup from previous crashes)
      const existingBot = await prisma.bot.findUnique({
        where: { clientId: clientId }
      })
      
      if (existingBot) {
        await prisma.bot.delete({
          where: { clientId: clientId }
        })
      }
      
      // Create bot in database
      const bot = await prisma.bot.create({
        data: {
          clientId: clientId,
          name: name, // Display name for the bot
          username: username, // Growtopia username
          status: botStatus,
          nodeId: node.id
        }
      })

      // Dapatkan informasi tambahan tentang node
      const nodeInfo = await prisma.node.findUnique({
        where: { id: node.id },
        select: {
          name: true,
          apiUrl: true,
          owner: {
            select: {
              username: true
            }
          }
        }
      })
      
      return NextResponse.json({
        message: 'Bot berhasil dibuat',
        bot: {
          ...bot,
          node: nodeInfo
        },
        status: botStatus === 'ONLINE' ? 'Bot berhasil terhubung ke server Growtopia' : 'Bot berhasil dibuat tetapi belum terhubung ke server Growtopia'
      })
    } catch (error) {
      console.error(`Error creating bot on node ${node.name}:`, error)
      
      // Menangani error timeout dengan pesan yang lebih informatif
      if (axios.isAxiosError(error) && error.code === 'ECONNABORTED') {
        return NextResponse.json(
          { 
            error: `Timeout saat menghubungi node ${node.name}. Server mungkin sibuk atau tidak merespon. Silakan coba lagi nanti.`,
            details: error.message
          },
          { status: 504 } // Gateway Timeout
        )
      }
      
      // Menangani error koneksi lainnya
      if (axios.isAxiosError(error) && !error.response) {
        return NextResponse.json(
          { 
            error: `Tidak dapat terhubung ke node ${node.name}. Pastikan node aktif dan dapat diakses.`,
            details: error.message
          },
          { status: 503 } // Service Unavailable
        )
      }
      
      // Menangani error respons dari server
      if (axios.isAxiosError(error) && error.response) {
        return NextResponse.json(
          { 
            error: `Node ${node.name} mengembalikan error: ${error.response.data?.error || error.response.statusText}`,
            details: error.response.data
          },
          { status: error.response.status || 500 }
        )
      }
      
      // Error lainnya
      return NextResponse.json(
        { 
          error: `Gagal membuat bot: ${error instanceof Error ? error.message : 'Unknown error'}`,
          details: error instanceof Error ? error.stack : undefined
        },
        { status: 500 }
      )
    }
  } catch (error) {
    console.error('Create bot error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}