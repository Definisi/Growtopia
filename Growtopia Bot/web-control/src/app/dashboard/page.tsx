'use client'

import { useEffect, useState } from 'react'
import {
  Grid,
  Card,
  CardContent,
  Typography,
  Box,
  CircularProgress,
  Alert,
  Button,
  Chip
} from '@mui/material'
import {
  Computer,
  SmartToy,
  People,
  CheckCircle,
  Error,
  Refresh
} from '@mui/icons-material'
import {
  BarChart,
  Bar,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
  PieChart,
  Pie,
  Cell
} from 'recharts'
import { useAuth } from '@/components/AuthProvider'
import DashboardLayout from '@/components/DashboardLayout'
import axios from 'axios'

interface DashboardStats {
  overview: {
    totalNodes: number
    activeNodes: number
    totalBots: number
    onlineBots: number
    totalUsers?: number
  }
  nodesByStatus: Array<{ status: string; count: number }>
  botsByStatus: Array<{ status: string; count: number }>
  activityChart: Array<{ date: string; count: number }>
  topNodes: Array<{
    id: string
    name: string
    botCount: number
    isActive: boolean
    owner: string
  }>
}

const COLORS = ['#0088FE', '#00C49F', '#FFBB28', '#FF8042']

function StatCard({ title, value, icon, color = 'primary' }: {
  title: string
  value: number | string
  icon: React.ReactNode
  color?: 'primary' | 'secondary' | 'success' | 'error' | 'warning'
}) {
  return (
    <Card>
      <CardContent>
        <Box sx={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
          <Box>
            <Typography color="text.secondary" gutterBottom>
              {title}
            </Typography>
            <Typography variant="h4" component="div">
              {value}
            </Typography>
          </Box>
          <Box sx={{ color: `${color}.main` }}>
            {icon}
          </Box>
        </Box>
      </CardContent>
    </Card>
  )
}

export default function DashboardPage() {
  const [stats, setStats] = useState<DashboardStats | null>(null)
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState('')
  const [mounted, setMounted] = useState(false)
  const { user } = useAuth()

  const fetchStats = async () => {
    try {
      setLoading(true)
      setError('')
      const response = await axios.get('/api/dashboard/stats')
      setStats(response.data)
    } catch (err: any) {
      setError(err.response?.data?.error || 'Gagal memuat statistik')
    } finally {
      setLoading(false)
    }
  }

  useEffect(() => {
    setMounted(true)
    fetchStats()
  }, [])

  if (!mounted || loading) {
    return (
      <DashboardLayout>
        <Box sx={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: 400 }}>
          <CircularProgress />
        </Box>
      </DashboardLayout>
    )
  }

  if (error) {
    return (
      <DashboardLayout>
        <Alert severity="error" action={
          <Button color="inherit" size="small" onClick={fetchStats}>
            Retry
          </Button>
        }>
          {error}
        </Alert>
      </DashboardLayout>
    )
  }

  if (!stats) {
    return (
      <DashboardLayout>
        <Alert severity="info">
          Tidak ada data statistik
        </Alert>
      </DashboardLayout>
    )
  }

  return (
    <DashboardLayout>
      <Box sx={{ mb: 3, display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
        <Typography variant="h4" component="h1">
          Dashboard
        </Typography>
        <Button
          variant="outlined"
          startIcon={<Refresh />}
          onClick={fetchStats}
          disabled={loading}
        >
          Refresh
        </Button>
      </Box>

      {/* Overview Stats */}
      <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 3, mb: 4 }}>
        <Box sx={{ width: { xs: '100%', sm: '45%', md: '22%' } }}>
          <StatCard
            title="Total Nodes"
            value={stats.overview.totalNodes}
            icon={<Computer sx={{ fontSize: 40 }} />}
            color="primary"
          />
        </Box>
        <Box sx={{ width: { xs: '100%', sm: '45%', md: '22%' } }}>
          <StatCard
            title="Active Nodes"
            value={stats.overview.activeNodes}
            icon={<CheckCircle sx={{ fontSize: 40 }} />}
            color="success"
          />
        </Box>
        <Box sx={{ width: { xs: '100%', sm: '45%', md: '22%' } }}>
          <StatCard
            title="Total Bots"
            value={stats.overview.totalBots}
            icon={<SmartToy sx={{ fontSize: 40 }} />}
            color="secondary"
          />
        </Box>
        <Box sx={{ width: { xs: '100%', sm: '45%', md: '22%' } }}>
          <StatCard
            title="Online Bots"
            value={stats.overview.onlineBots}
            icon={<CheckCircle sx={{ fontSize: 40 }} />}
            color="success"
          />
        </Box>
        {user?.role === 'ADMIN' && stats.overview.totalUsers !== undefined && (
          <Box sx={{ width: { xs: '100%', sm: '45%', md: '22%' } }}>
            <StatCard
              title="Total Users"
              value={stats.overview.totalUsers}
              icon={<People sx={{ fontSize: 40 }} />}
              color="warning"
            />
          </Box>
        )}
      </Box>

      <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 3 }}>
        {/* Activity Chart */}
        <Box sx={{ width: { xs: '100%', md: '65%' } }}>
          <Card>
            <CardContent>
              <Typography variant="h6" gutterBottom>
                Aktivitas Node (7 Hari Terakhir)
              </Typography>
              <ResponsiveContainer width="100%" height={300}>
                <BarChart data={stats.activityChart}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis 
                    dataKey="date" 
                    tickFormatter={(value) => {
                      return new Date(value).toLocaleDateString('id-ID', { month: 'short', day: 'numeric' })
                    }}
                    suppressHydrationWarning={true}
                  />
                  <YAxis />
                  <Tooltip 
                    labelFormatter={(value) => new Date(value).toLocaleDateString('id-ID')}
                  />
                  <Bar dataKey="count" fill="#1976d2" />
                </BarChart>
              </ResponsiveContainer>
            </CardContent>
          </Card>
        </Box>

        {/* Bot Status Pie Chart */}
        <Box sx={{ width: { xs: '100%', md: '30%' } }}>
          <Card>
            <CardContent>
              <Typography variant="h6" gutterBottom>
                Status Bot
              </Typography>
              <ResponsiveContainer width="100%" height={300}>
                <PieChart>
                  <Pie
                    data={stats.botsByStatus}
                    cx="50%"
                    cy="50%"
                    labelLine={false}
                    label={({ status, count }) => `${status}: ${count}`}
                    outerRadius={80}
                    fill="#8884d8"
                    dataKey="count"
                  >
                    {stats.botsByStatus.map((entry, index) => (
                      <Cell key={`cell-${index}`} fill={COLORS[index % COLORS.length]} />
                    ))}
                  </Pie>
                  <Tooltip />
                </PieChart>
              </ResponsiveContainer>
            </CardContent>
          </Card>
        </Box>

        {/* Top Nodes */}
        <Box sx={{ width: '100%' }}>
          <Card>
            <CardContent>
              <Typography variant="h6" gutterBottom>
                Top Nodes (Berdasarkan Jumlah Bot)
              </Typography>
              <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 2 }}>
                {stats.topNodes.map((node) => (
                  <Box sx={{ width: { xs: '100%', sm: '45%', md: '30%' } }} key={node.id}>
                    <Card variant="outlined">
                      <CardContent>
                        <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start', mb: 1 }}>
                          <Typography variant="h6" component="div">
                            {node.name}
                          </Typography>
                          <Chip
                            label={node.isActive ? 'Active' : 'Inactive'}
                            color={node.isActive ? 'success' : 'error'}
                            size="small"
                          />
                        </Box>
                        <Typography color="text.secondary" gutterBottom suppressHydrationWarning={true}>
                          Owner: {node.owner}
                        </Typography>
                        <Typography variant="h4" color="primary">
                          {node.botCount}
                        </Typography>
                        <Typography color="text.secondary">
                          Bot{node.botCount !== 1 ? 's' : ''}
                        </Typography>
                      </CardContent>
                    </Card>
                  </Box>
                ))}
              </Box>
            </CardContent>
          </Card>
        </Box>
      </Box>
    </DashboardLayout>
  )
}