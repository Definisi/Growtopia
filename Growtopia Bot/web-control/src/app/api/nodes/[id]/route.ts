import { NextRequest, NextResponse } from 'next/server'
import { prisma } from '@/lib/prisma'
import { getAuthUser } from '@/lib/auth'

// GET - Get single node
export async function GET(
  request: NextRequest,
  { params }: { params: Promise<{ id: string }> }
) {
  const { id } = await params
  try {
    const user = await getAuthUser(request)
    if (!user) {
      return NextResponse.json({ error: 'Unauthorized' }, { status: 401 })
    }

    const where = user.role === 'ADMIN' 
      ? { id: id }
      : { id: id, ownerId: user.id }

    const node = await prisma.node.findFirst({
      where,
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

    if (!node) {
      return NextResponse.json({ error: 'Node tidak ditemukan' }, { status: 404 })
    }

    return NextResponse.json(node)
  } catch (error) {
    console.error('Get node error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}

// PUT - Update node
export async function PUT(
  request: NextRequest,
  { params }: { params: Promise<{ id: string }> }
) {
  const { id } = await params
  try {
    const user = await getAuthUser(request)
    if (!user) {
      return NextResponse.json({ error: 'Unauthorized' }, { status: 401 })
    }

    const { name, apiUrl, isActive } = await request.json()

    const where = user.role === 'ADMIN' 
      ? { id: id }
      : { id: id, ownerId: user.id }

    const existingNode = await prisma.node.findFirst({ where })
    if (!existingNode) {
      return NextResponse.json({ error: 'Node tidak ditemukan' }, { status: 404 })
    }

    const node = await prisma.node.update({
      where: { id: id },
      data: {
        ...(name && { name }),
        ...(apiUrl && { apiUrl }),
        ...(typeof isActive === 'boolean' && { isActive }),
        updatedAt: new Date()
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
    console.error('Update node error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}

// DELETE - Delete node
export async function DELETE(
  request: NextRequest,
  { params }: { params: Promise<{ id: string }> }
) {
  const { id } = await params
  try {
    const user = await getAuthUser(request)
    if (!user) {
      return NextResponse.json({ error: 'Unauthorized' }, { status: 401 })
    }

    const where = user.role === 'ADMIN' 
      ? { id: id }
      : { id: id, ownerId: user.id }

    const existingNode = await prisma.node.findFirst({ where })
    if (!existingNode) {
      return NextResponse.json({ error: 'Node tidak ditemukan' }, { status: 404 })
    }

    await prisma.node.delete({
      where: { id: id }
    })

    return NextResponse.json({ message: 'Node berhasil dihapus' })
  } catch (error) {
    console.error('Delete node error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}