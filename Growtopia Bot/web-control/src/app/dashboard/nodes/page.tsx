'use client'

import { useEffect, useState, useCallback } from 'react'
import {
  Box,
  Typography,
  Button,
  Card,
  CardContent,
  Grid,
  Chip,
  IconButton,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  TextField,
  FormControlLabel,
  Switch,
  Menu,
  MenuItem,
  Alert,
  CircularProgress,
  Tooltip
} from '@mui/material'
import {
  Add as AddIcon,
  MoreVert as MoreVertIcon,
  Edit as EditIcon,
  Delete as DeleteIcon,
  Computer as ComputerIcon,
  CheckCircle as CheckCircleIcon,
  Cancel as CancelIcon,
  Refresh as RefreshIcon
} from '@mui/icons-material'
import { useAuth } from '@/components/AuthProvider'
import DashboardLayout from '@/components/DashboardLayout'
import axios from 'axios'

interface Node {
  id: string
  name: string
  apiUrl: string
  isActive: boolean
  createdAt: string
  updatedAt: string
  bots?: {
    id: string
    username: string
    status: string
  }[]
  owner?: {
    username: string
    email: string
  }
}

interface NodeFormData {
  name: string
  apiUrl: string
  isActive: boolean
}

export default function DashboardNodesPage() {
  const [nodes, setNodes] = useState<Node[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState('')
  const [dialogOpen, setDialogOpen] = useState(false)
  const [editingNode, setEditingNode] = useState<Node | null>(null)
  const [formData, setFormData] = useState<NodeFormData>({
    name: '',
    apiUrl: '',
    isActive: true
  })
  const [submitting, setSubmitting] = useState(false)
  const [anchorEl, setAnchorEl] = useState<null | HTMLElement>(null)
  const [selectedNode, setSelectedNode] = useState<Node | null>(null)
  const [mounted, setMounted] = useState(false)
  const { user } = useAuth()

  const fetchNodes = useCallback(async () => {
    try {
      setLoading(true)
      setError('')
      const response = await axios.get('/api/nodes')
      setNodes(response.data)
    } catch (err: any) {
      setError(err.response?.data?.error || 'Gagal memuat nodes')
    } finally {
      setLoading(false)
    }
  }, [])
  
  const checkNodeStatus = useCallback(async () => {
    try {
      await axios.post('/api/nodes/check')
      // Refresh nodes after status check
      fetchNodes()
    } catch (err: any) {
      console.error('Error checking node status:', err)
      // Don't show error to user, just log it
    }
  }, [fetchNodes]) // Include fetchNodes as dependency

  useEffect(() => {
    setMounted(true)
    fetchNodes()
    
    // Set up interval to check node status every 30 seconds
    const intervalId = setInterval(() => {
      checkNodeStatus()
    }, 30000)
    
    // Clean up interval on component unmount
    return () => clearInterval(intervalId)
  }, [checkNodeStatus]) // Now checkNodeStatus won't change on every render

  const handleOpenDialog = (node?: Node) => {
    if (node) {
      setEditingNode(node)
      setFormData({
        name: node.name,
        apiUrl: node.apiUrl,
        isActive: node.isActive
      })
    } else {
      setEditingNode(null)
      setFormData({
        name: '',
        apiUrl: '',
        isActive: true
      })
    }
    setDialogOpen(true)
  }

  const handleCloseDialog = () => {
    setDialogOpen(false)
    setEditingNode(null)
    setFormData({
      name: '',
      apiUrl: '',
      isActive: true
    })
  }

  const handleSubmit = async () => {
    try {
      setSubmitting(true)
      setError('')

      if (editingNode) {
        // Update existing node
        await axios.put(`/api/nodes/${editingNode.id}`, formData)
      } else {
        // Create new node
        await axios.post('/api/nodes', formData)
      }

      handleCloseDialog()
      fetchNodes()
    } catch (err: any) {
      setError(err.response?.data?.error || 'Gagal menyimpan node')
    } finally {
      setSubmitting(false)
    }
  }

  const handleDelete = async (nodeId: string) => {
    if (!confirm('Apakah Anda yakin ingin menghapus node ini?')) {
      return
    }

    try {
      setError('')
      await axios.delete(`/api/nodes/${nodeId}`)
      fetchNodes()
      handleCloseMenu()
    } catch (err: any) {
      setError(err.response?.data?.error || 'Gagal menghapus node')
    }
  }

  const handleOpenMenu = (event: React.MouseEvent<HTMLElement>, node: Node) => {
    setAnchorEl(event.currentTarget)
    setSelectedNode(node)
  }

  const handleCloseMenu = () => {
    setAnchorEl(null)
    setSelectedNode(null)
  }

  if (!mounted) {
    return null
  }

  if (loading) {
    return (
      <Box display="flex" justifyContent="center" alignItems="center" minHeight="400px">
        <CircularProgress />
      </Box>
    )
  }

  return (
    <DashboardLayout>
      <Box>
        <Box display="flex" justifyContent="space-between" alignItems="center" mb={3}>
        <Typography variant="h4" component="h1">
          Node Management
        </Typography>
        <Box display="flex" gap={2}>
          <Button
            variant="outlined"
            onClick={checkNodeStatus}
            startIcon={<RefreshIcon />}
          >
            Check Status
          </Button>
          <Button
            variant="contained"
            startIcon={<AddIcon />}
            onClick={() => handleOpenDialog()}
          >
            Tambah Node
          </Button>
        </Box>
      </Box>

      {error && (
        <Alert severity="error" sx={{ mb: 2 }}>
          {error}
        </Alert>
      )}

      <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 3 }}>
        {nodes.map((node) => (
          <Box sx={{ width: { xs: '100%', sm: '45%', lg: '30%' } }} key={node.id}>
            <Card>
              <CardContent>
                <Box display="flex" justifyContent="space-between" alignItems="flex-start" mb={2}>
                  <Box display="flex" alignItems="center" gap={1}>
                    <ComputerIcon color="primary" />
                    <Typography variant="h6" component="h2">
                      {node.name}
                    </Typography>
                  </Box>
                  <Box>
                    <Chip
                      icon={node.isActive ? <CheckCircleIcon /> : <CancelIcon />}
                      label={node.isActive ? 'Active' : 'Inactive'}
                      color={node.isActive ? 'success' : 'default'}
                      size="small"
                    />
                    <IconButton
                      size="small"
                      onClick={(e) => handleOpenMenu(e, node)}
                    >
                      <MoreVertIcon />
                    </IconButton>
                  </Box>
                </Box>

                <Typography variant="body2" color="text.secondary" gutterBottom>
                  API URL: {node.apiUrl}
                </Typography>

                <Box display="flex" justifyContent="space-between" alignItems="center" mt={2}>
                  <Typography variant="body2" color="text.secondary">
                    Bots: {node.bots ? node.bots.length : 0}
                  </Typography>
                  {node.owner && (
                    <Typography variant="body2" color="text.secondary">
                      Owner: {node.owner.username}
                    </Typography>
                  )}
                </Box>

                <Typography variant="caption" color="text.secondary" display="block" mt={1} suppressHydrationWarning>
                  Created: {new Date(node.createdAt).toLocaleDateString('id-ID')}
                </Typography>
              </CardContent>
            </Card>
          </Box>
        ))}
      </Box>

      {nodes.length === 0 && !loading && (
        <Box textAlign="center" py={4}>
          <Typography variant="h6" color="text.secondary" gutterBottom>
            Belum ada node
          </Typography>
          <Typography variant="body2" color="text.secondary" mb={2}>
            Tambahkan node pertama Anda untuk mulai mengelola bot
          </Typography>
          <Button
            variant="contained"
            startIcon={<AddIcon />}
            onClick={() => handleOpenDialog()}
          >
            Tambah Node
          </Button>
        </Box>
      )}

      {/* Node Form Dialog */}
      <Dialog open={dialogOpen} onClose={handleCloseDialog} maxWidth="sm" fullWidth>
        <DialogTitle>
          {editingNode ? 'Edit Node' : 'Tambah Node Baru'}
        </DialogTitle>
        <DialogContent>
          <Box component="form" sx={{ mt: 1 }}>
            <TextField
              fullWidth
              label="Nama Node"
              value={formData.name}
              onChange={(e) => setFormData({ ...formData, name: e.target.value })}
              margin="normal"
              required
            />
            <TextField
              fullWidth
              label="API URL"
              value={formData.apiUrl}
              onChange={(e) => setFormData({ ...formData, apiUrl: e.target.value })}
              margin="normal"
              required
              placeholder="http://localhost:8080"
            />
            <FormControlLabel
              control={
                <Switch
                  checked={formData.isActive}
                  onChange={(e) => setFormData({ ...formData, isActive: e.target.checked })}
                />
              }
              label="Node Aktif"
              sx={{ mt: 2 }}
            />
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={handleCloseDialog}>Batal</Button>
          <Button
            onClick={handleSubmit}
            variant="contained"
            disabled={submitting || !formData.name || !formData.apiUrl}
          >
            {submitting ? 'Menyimpan...' : (editingNode ? 'Update' : 'Tambah')}
          </Button>
        </DialogActions>
      </Dialog>

      {/* Context Menu */}
      <Menu
        anchorEl={anchorEl}
        open={Boolean(anchorEl)}
        onClose={handleCloseMenu}
      >
        <MenuItem onClick={() => {
          if (selectedNode) {
            handleOpenDialog(selectedNode)
            handleCloseMenu()
          }
        }}>
          <EditIcon sx={{ mr: 1 }} fontSize="small" />
          Edit
        </MenuItem>
        <MenuItem 
          onClick={() => selectedNode && handleDelete(selectedNode.id)}
          sx={{ color: 'error.main' }}
        >
          <DeleteIcon sx={{ mr: 1 }} fontSize="small" />
          Hapus
        </MenuItem>
      </Menu>
    </Box>
    </DashboardLayout>
  )
}