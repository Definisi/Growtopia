import { NextRequest, NextResponse } from 'next/server'
import { prisma } from '@/lib/prisma'
import axios from 'axios'

// POST - Check all nodes status
export async function POST(request: NextRequest) {
  try {
    // Get all nodes
    const nodes = await prisma.node.findMany()
    
    let activeCount = 0
    let inactiveCount = 0
    const errors: string[] = []

    for (const node of nodes) {
      try {
        // Try to connect to node API
        const response = await axios.get(`${node.apiUrl}/api/1.0/status`, {
          timeout: 5000 // 5 seconds timeout
        })
        
        // If we get here, the node is active
        if (!node.isActive) {
          // Update node status to active
          await prisma.node.update({
            where: { id: node.id },
            data: { isActive: true }
          })
        }
        activeCount++
      } catch (error) {
        // Node is not responding
        if (node.isActive) {
          // Update node status to inactive
          await prisma.node.update({
            where: { id: node.id },
            data: { isActive: false }
          })
        }
        inactiveCount++
        errors.push(`Node ${node.name}: ${error instanceof Error ? error.message : 'Unknown error'}`)
      }
    }

    return NextResponse.json({
      message: `Checked ${nodes.length} nodes. Active: ${activeCount}, Inactive: ${inactiveCount}`,
      activeCount,
      inactiveCount,
      errors
    })
  } catch (error) {
    console.error('Check nodes error:', error)
    return NextResponse.json(
      { error: 'Terjadi kesalahan server' },
      { status: 500 }
    )
  }
}