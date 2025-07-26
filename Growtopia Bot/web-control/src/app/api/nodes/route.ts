import { NextRequest, NextResponse } from 'next/server'
import { prisma } from '@/lib/prisma'
import { getAuthUser } from '@/lib/auth'

// GET - List all nodes for authenticated user
export async function GET(request: NextRequest) {
  try {
    const user = await getAuthUser(request)
    if (!user) {
      return NextResponse.json({ error: 'Unauthorized' }, { status: 401 })
    }

    const where = user.role === 'ADMIN' ? {} : { ownerId: user.id }
    
    const nodes = await prisma.node.findMany({
      where,
      include: {
        owner: {
          select: {
            id: true,
            username: true,
            email: true
          }
        },
        bots: {
          select: {
            id: true,
            username: true,
            status: true
          }
        }
      },
      orderBy: {
        createdAt: 'desc'
      }
    })

    return NextResponse.json(nodes)
  } catch (error) {
    console.error('Get nodes error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}

// POST - Create new node
export async function POST(request: NextRequest) {
  try {
    const user = await getAuthUser(request)
    if (!user) {
      return NextResponse.json({ error: 'Unauthorized' }, { status: 401 })
    }

    const { name, apiUrl } = await request.json()

    if (!name || !apiUrl) {
      return NextResponse.json(
        { error: 'Name dan API URL harus diisi' },
        { status: 400 }
      )
    }

    // Check node limit for freemium users
    if (user.role === 'FREEMIUM') {
      const nodeCount = await prisma.node.count({
        where: { ownerId: user.id }
      })
      
      if (nodeCount >= 3) {
        return NextResponse.json(
          { error: 'Akun freemium hanya dapat memiliki maksimal 3 node' },
          { status: 403 }
        )
      }
    }

    const node = await prisma.node.create({
      data: {
        name,
        apiUrl,
        ownerId: user.id
      },
      include: {
        owner: {
          select: {
            id: true,
            username: true,
            email: true
          }
        },
        bots: true
      }
    })

    return NextResponse.json(node)
  } catch (error) {
    console.error('Create node error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}