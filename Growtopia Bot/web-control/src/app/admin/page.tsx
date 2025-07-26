'use client'

import { useEffect, useState } from 'react'
import {
  Box,
  Typography,
  Card,
  CardContent,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Paper,
  Chip,
  IconButton,
  Menu,
  MenuItem,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Button,
  FormControl,
  InputLabel,
  Select,
  Alert,
  CircularProgress,
  Avatar,
  Switch,
  FormControlLabel
} from '@mui/material'
import {
  MoreVert,
  Edit,
  Delete,
  Person,
  AdminPanelSettings,
  Star,
  Refresh
} from '@mui/icons-material'
import { useAuth } from '@/components/AuthProvider'
import DashboardLayout from '@/components/DashboardLayout'
import { useRouter } from 'next/navigation'
import axios from 'axios'

interface User {
  id: string
  username: string
  email: string
  role: 'ADMIN' | 'PREMIUM' | 'FREEMIUM'
  isActive: boolean
  createdAt: string
  updatedAt: string
  _count: {
    nodes: number
  }
}

interface EditUserData {
  role: 'ADMIN' | 'PREMIUM' | 'FREEMIUM'
  isActive: boolean
}

export default function AdminPage() {
  const [users, setUsers] = useState<User[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState('')
  const [anchorEl, setAnchorEl] = useState<null | HTMLElement>(null)
  const [selectedUser, setSelectedUser] = useState<User | null>(null)
  const [editDialog, setEditDialog] = useState(false)
  const [editData, setEditData] = useState<EditUserData>({
    role: 'FREEMIUM',
    isActive: true
  })
  const [submitting, setSubmitting] = useState(false)
  const { user } = useAuth()
  const router = useRouter()

  // Redirect if not admin
  useEffect(() => {
    if (user && user.role !== 'ADMIN') {
      router.push('/dashboard')
    }
  }, [user, router])

  const fetchUsers = async () => {
    try {
      setLoading(true)
      setError('')
      const response = await axios.get('/api/admin/users')
      setUsers(response.data)
    } catch (err: any) {
      setError(err.response?.data?.error || 'Gagal memuat users')
    } finally {
      setLoading(false)
    }
  }

  useEffect(() => {
    if (user?.role === 'ADMIN') {
      fetchUsers()
    }
  }, [user])

  const handleMenuClick = (event: React.MouseEvent<HTMLElement>, selectedUser: User) => {
    setAnchorEl(event.currentTarget)
    setSelectedUser(selectedUser)
  }

  const handleCloseMenu = () => {
    setAnchorEl(null)
    setSelectedUser(null)
  }

  const handleEditUser = () => {
    if (!selectedUser) return
    
    setEditData({
      role: selectedUser.role,
      isActive: selectedUser.isActive
    })
    setEditDialog(true)
    handleCloseMenu()
  }

  const handleUpdateUser = async () => {
    if (!selectedUser) return

    try {
      setSubmitting(true)
      setError('')
      
      await axios.put('/api/admin/users', {
        userId: selectedUser.id,
        ...editData
      })

      setEditDialog(false)
      fetchUsers()
    } catch (err: any) {
      setError(err.response?.data?.error || 'Gagal mengupdate user')
    } finally {
      setSubmitting(false)
    }
  }

  const handleDeleteUser = async () => {
    if (!selectedUser) return

    if (!confirm(`Apakah Anda yakin ingin menghapus user ${selectedUser.username}?`)) {
      return
    }

    try {
      await axios.delete('/api/admin/users', {
        data: { userId: selectedUser.id }
      })
      
      fetchUsers()
      handleCloseMenu()
    } catch (err: any) {
      setError(err.response?.data?.error || 'Gagal menghapus user')
    }
  }

  const getRoleIcon = (role: string) => {
    switch (role) {
      case 'ADMIN': return <AdminPanelSettings />
      case 'PREMIUM': return <Star />
      case 'FREEMIUM': return <Person />
      default: return <Person />
    }
  }

  const getRoleColor = (role: string) => {
    switch (role) {
      case 'ADMIN': return 'error'
      case 'PREMIUM': return 'warning'
      case 'FREEMIUM': return 'default'
      default: return 'default'
    }
  }

  if (user?.role !== 'ADMIN') {
    return (
      <DashboardLayout>
        <Alert severity="error">
          Anda tidak memiliki akses ke halaman ini.
        </Alert>
      </DashboardLayout>
    )
  }

  if (loading) {
    return (
      <DashboardLayout>
        <Box sx={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: 400 }}>
          <CircularProgress />
        </Box>
      </DashboardLayout>
    )
  }

  return (
    <DashboardLayout>
      <Box sx={{ mb: 3, display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
        <Typography variant="h4" component="h1">
          User Management
        </Typography>
        <Button
          variant="outlined"
          startIcon={<Refresh />}
          onClick={fetchUsers}
          disabled={loading}
        >
          Refresh
        </Button>
      </Box>

      {error && (
        <Alert severity="error" sx={{ mb: 3 }}>
          {error}
        </Alert>
      )}

      <Card>
        <CardContent>
          <TableContainer component={Paper} variant="outlined">
            <Table>
              <TableHead>
                <TableRow>
                  <TableCell>User</TableCell>
                  <TableCell>Email</TableCell>
                  <TableCell>Role</TableCell>
                  <TableCell>Status</TableCell>
                  <TableCell>Nodes</TableCell>
                  <TableCell>Created</TableCell>
                  <TableCell>Actions</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {users.map((userData) => (
                  <TableRow key={userData.id}>
                    <TableCell>
                      <Box sx={{ display: 'flex', alignItems: 'center', gap: 2 }}>
                        <Avatar sx={{ bgcolor: 'primary.main' }}>
                          {userData.username.charAt(0).toUpperCase()}
                        </Avatar>
                        <Typography variant="body2">
                          {userData.username}
                        </Typography>
                      </Box>
                    </TableCell>
                    <TableCell>{userData.email}</TableCell>
                    <TableCell>
                      <Chip
                        label={userData.role}
                        color={getRoleColor(userData.role) as any}
                        size="small"
                        icon={getRoleIcon(userData.role)}
                      />
                    </TableCell>
                    <TableCell>
                      <Chip
                        label={userData.isActive ? 'Active' : 'Inactive'}
                        color={userData.isActive ? 'success' : 'error'}
                        size="small"
                      />
                    </TableCell>
                    <TableCell>{userData._count.nodes}</TableCell>
                    <TableCell>
                      {new Date(userData.createdAt).toLocaleDateString('id-ID')}
                    </TableCell>
                    <TableCell>
                      <IconButton
                        size="small"
                        onClick={(e) => handleMenuClick(e, userData)}
                        disabled={userData.id === user?.id}
                      >
                        <MoreVert />
                      </IconButton>
                    </TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          </TableContainer>

          {users.length === 0 && (
            <Box sx={{ textAlign: 'center', py: 4 }}>
              <Typography color="text.secondary">
                Tidak ada users ditemukan
              </Typography>
            </Box>
          )}
        </CardContent>
      </Card>

      {/* Context Menu */}
      <Menu
        anchorEl={anchorEl}
        open={Boolean(anchorEl)}
        onClose={handleCloseMenu}
      >
        <MenuItem onClick={handleEditUser}>
          <Edit sx={{ mr: 1 }} /> Edit User
        </MenuItem>
        <MenuItem onClick={handleDeleteUser}>
          <Delete sx={{ mr: 1 }} /> Delete User
        </MenuItem>
      </Menu>

      {/* Edit User Dialog */}
      <Dialog open={editDialog} onClose={() => setEditDialog(false)} maxWidth="sm" fullWidth>
        <DialogTitle>
          Edit User: {selectedUser?.username}
        </DialogTitle>
        <DialogContent>
          <FormControl fullWidth margin="dense">
            <InputLabel>Role</InputLabel>
            <Select
              value={editData.role}
              label="Role"
              onChange={(e) => setEditData({ ...editData, role: e.target.value as any })}
            >
              <MenuItem value="FREEMIUM">Freemium</MenuItem>
              <MenuItem value="PREMIUM">Premium</MenuItem>
              <MenuItem value="ADMIN">Admin</MenuItem>
            </Select>
          </FormControl>
          
          <FormControlLabel
            control={
              <Switch
                checked={editData.isActive}
                onChange={(e) => setEditData({ ...editData, isActive: e.target.checked })}
              />
            }
            label="User Active"
            sx={{ mt: 2 }}
          />
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setEditDialog(false)}>Batal</Button>
          <Button
            onClick={handleUpdateUser}
            variant="contained"
            disabled={submitting}
          >
            {submitting ? <CircularProgress size={20} /> : 'Update'}
          </Button>
        </DialogActions>
      </Dialog>
    </DashboardLayout>
  )
}