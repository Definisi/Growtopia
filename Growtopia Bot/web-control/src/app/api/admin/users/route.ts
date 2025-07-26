import { NextRequest, NextResponse } from 'next/server'
import { prisma } from '@/lib/prisma'
import { getAuthUser, checkPermission, hashPassword } from '@/lib/auth'

// GET - List all users (Admin only)
export async function GET(request: NextRequest) {
  try {
    const user = await getAuthUser(request)
    if (!user || !checkPermission(user.role, 'ADMIN')) {
      return NextResponse.json({ error: 'Forbidden' }, { status: 403 })
    }

    const users = await prisma.user.findMany({
      select: {
        id: true,
        email: true,
        username: true,
        role: true,
        isActive: true,
        createdAt: true,
        updatedAt: true,
        _count: {
          select: {
            nodes: true
          }
        }
      },
      orderBy: {
        createdAt: 'desc'
      }
    })

    return NextResponse.json(users)
  } catch (error) {
    console.error('Get users error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}

// POST - Create new user (Admin only)
export async function POST(request: NextRequest) {
  try {
    const user = await getAuthUser(request)
    if (!user || !checkPermission(user.role, 'ADMIN')) {
      return NextResponse.json({ error: 'Forbidden' }, { status: 403 })
    }

    const { email, username, password, role, isActive } = await request.json()

    if (!email || !username || !password) {
      return NextResponse.json(
        { error: 'Email, username, dan password harus diisi' },
        { status: 400 }
      )
    }

    // Check if email or username already exists
    const existingUser = await prisma.user.findFirst({
      where: {
        OR: [
          { email },
          { username }
        ]
      }
    })

    if (existingUser) {
      return NextResponse.json(
        { error: 'Email atau username sudah digunakan' },
        { status: 400 }
      )
    }

    const hashedPassword = await hashPassword(password)

    const newUser = await prisma.user.create({
      data: {
        email,
        username,
        password: hashedPassword,
        role: role || 'FREEMIUM',
        isActive: isActive !== undefined ? isActive : true
      },
      select: {
        id: true,
        email: true,
        username: true,
        role: true,
        isActive: true,
        createdAt: true,
        updatedAt: true,
        _count: {
          select: {
            nodes: true
          }
        }
      }
    })

    return NextResponse.json(newUser)
  } catch (error) {
    console.error('Create user error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}

// PUT - Update user role/status (Admin only)
export async function PUT(request: NextRequest) {
  try {
    const user = await getAuthUser(request)
    if (!user || !checkPermission(user.role, 'ADMIN')) {
      return NextResponse.json({ error: 'Forbidden' }, { status: 403 })
    }

    const { userId, role, isActive } = await request.json()

    if (!userId) {
      return NextResponse.json(
        { error: 'User ID harus diisi' },
        { status: 400 }
      )
    }

    // Prevent admin from changing their own role
    if (userId === user.id) {
      return NextResponse.json(
        { error: 'Tidak dapat mengubah role sendiri' },
        { status: 400 }
      )
    }

    const updateData: any = {}
    if (role && ['ADMIN', 'PREMIUM', 'FREEMIUM'].includes(role)) {
      updateData.role = role
    }
    if (typeof isActive === 'boolean') {
      updateData.isActive = isActive
    }

    const updatedUser = await prisma.user.update({
      where: { id: userId },
      data: updateData,
      select: {
        id: true,
        email: true,
        username: true,
        role: true,
        isActive: true,
        createdAt: true,
        _count: {
          select: {
            nodes: true
          }
        }
      }
    })

    return NextResponse.json(updatedUser)
  } catch (error) {
    console.error('Update user error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}

// DELETE - Delete user (Admin only)
export async function DELETE(request: NextRequest) {
  try {
    const user = await getAuthUser(request)
    if (!user || !checkPermission(user.role, 'ADMIN')) {
      return NextResponse.json({ error: 'Forbidden' }, { status: 403 })
    }

    const { userId } = await request.json()

    if (!userId) {
      return NextResponse.json(
        { error: 'User ID harus diisi' },
        { status: 400 }
      )
    }

    // Prevent admin from deleting themselves
    if (userId === user.id) {
      return NextResponse.json(
        { error: 'Tidak dapat menghapus akun sendiri' },
        { status: 400 }
      )
    }

    await prisma.user.delete({
      where: { id: userId }
    })

    return NextResponse.json({ message: 'User berhasil dihapus' })
  } catch (error) {
    console.error('Delete user error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}