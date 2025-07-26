'use client'

import { useEffect, useState } from 'react'
import {
  Box,
  Typography,
  Card,
  CardContent,
  Grid,
  Chip,
  IconButton,
  Menu,
  MenuItem,
  Alert,
  CircularProgress,
  Button,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  TextField,
  FormControl,
  InputLabel,
  Select,
  MenuItem as SelectMenuItem,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Paper,
  Avatar
} from '@mui/material'
import {
  MoreVert as MoreVertIcon,
  Edit as EditIcon,
  Delete as DeleteIcon,
  Person as PersonIcon,
  Add as AddIcon,
  AdminPanelSettings as AdminIcon,
  Star as PremiumIcon,
  AccountCircle as FreemiumIcon
} from '@mui/icons-material'
import { useAuth } from '@/components/AuthProvider'
import axios from 'axios'

interface User {
  id: string
  email: string
  username: string
  role: 'ADMIN' | 'PREMIUM' | 'FREEMIUM'
  isActive: boolean
  createdAt: string
  updatedAt: string
  _count?: {
    nodes: number
  }
}

interface UserFormData {
  email: string
  username: string
  password: string
  role: 'ADMIN' | 'PREMIUM' | 'FREEMIUM'
  isActive: boolean
}

export default function DashboardUsersPage() {
  const [users, setUsers] = useState<User[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState('')
  const [anchorEl, setAnchorEl] = useState<null | HTMLElement>(null)
  const [selectedUser, setSelectedUser] = useState<User | null>(null)
  const [dialogOpen, setDialogOpen] = useState(false)
  const [editingUser, setEditingUser] = useState<User | null>(null)
  const [formData, setFormData] = useState<UserFormData>({
    email: '',
    username: '',
    password: '',
    role: 'FREEMIUM',
    isActive: true
  })
  const [submitting, setSubmitting] = useState(false)
  const [mounted, setMounted] = useState(false)
  const { user } = useAuth()

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
    setMounted(true)
    if (user?.role === 'ADMIN') {
      fetchUsers()
    }
  }, [user])

  const handleOpenDialog = (userToEdit?: User) => {
    if (userToEdit) {
      setEditingUser(userToEdit)
      setFormData({
        email: userToEdit.email,
        username: userToEdit.username,
        password: '',
        role: userToEdit.role,
        isActive: userToEdit.isActive
      })
    } else {
      setEditingUser(null)
      setFormData({
        email: '',
        username: '',
        password: '',
        role: 'FREEMIUM',
        isActive: true
      })
    }
    setDialogOpen(true)
  }

  const handleCloseDialog = () => {
    setDialogOpen(false)
    setEditingUser(null)
    setFormData({
      email: '',
      username: '',
      password: '',
      role: 'FREEMIUM',
      isActive: true
    })
  }

  const handleSubmit = async () => {
    try {
      setSubmitting(true)
      setError('')

      if (editingUser) {
        // Update existing user
        const updateData = {
          email: formData.email,
          username: formData.username,
          role: formData.role,
          isActive: formData.isActive,
          ...(formData.password && { password: formData.password })
        }
        await axios.put(`/api/admin/users/${editingUser.id}`, updateData)
      } else {
        // Create new user
        await axios.post('/api/admin/users', formData)
      }

      handleCloseDialog()
      fetchUsers()
    } catch (err: any) {
      setError(err.response?.data?.error || 'Gagal menyimpan user')
    } finally {
      setSubmitting(false)
    }
  }

  const handleDeleteUser = async (userId: string) => {
    if (!confirm('Apakah Anda yakin ingin menghapus user ini?')) {
      return
    }

    try {
      setError('')
      await axios.delete(`/api/admin/users/${userId}`)
      fetchUsers()
      handleCloseMenu()
    } catch (err: any) {
      setError(err.response?.data?.error || 'Gagal menghapus user')
    }
  }

  const handleOpenMenu = (event: React.MouseEvent<HTMLElement>, userItem: User) => {
    setAnchorEl(event.currentTarget)
    setSelectedUser(userItem)
  }

  const handleCloseMenu = () => {
    setAnchorEl(null)
    setSelectedUser(null)
  }

  const getRoleIcon = (role: string) => {
    switch (role) {
      case 'ADMIN':
        return <AdminIcon />
      case 'PREMIUM':
        return <PremiumIcon />
      case 'FREEMIUM':
        return <FreemiumIcon />
      default:
        return <PersonIcon />
    }
  }

  const getRoleColor = (role: string) => {
    switch (role) {
      case 'ADMIN':
        return 'error'
      case 'PREMIUM':
        return 'warning'
      case 'FREEMIUM':
        return 'success'
      default:
        return 'default'
    }
  }

  if (user?.role !== 'ADMIN') {
    return (
      <Box display="flex" justifyContent="center" alignItems="center" minHeight="400px">
        <Alert severity="error">
          Akses ditolak. Hanya admin yang dapat mengakses halaman ini.
        </Alert>
      </Box>
    )
  }

  if (loading && mounted) {
    return (
      <Box display="flex" justifyContent="center" alignItems="center" minHeight="400px">
        <CircularProgress />
      </Box>
    )
  }

  return (
    <Box>
      <Box display="flex" justifyContent="space-between" alignItems="center" mb={3}>
        <Typography variant="h4" component="h1">
          User Management
        </Typography>
        <Button
          variant="contained"
          startIcon={<AddIcon />}
          onClick={() => handleOpenDialog()}
        >
          Tambah User
        </Button>
      </Box>

      {error && (
        <Alert severity="error" sx={{ mb: 2 }}>
          {error}
        </Alert>
      )}

      <TableContainer component={Paper}>
        <Table>
          <TableHead>
            <TableRow>
              <TableCell>User</TableCell>
              <TableCell>Email</TableCell>
              <TableCell>Role</TableCell>
              <TableCell>Status</TableCell>
              <TableCell>Nodes</TableCell>
              <TableCell>Created</TableCell>
              <TableCell align="right">Actions</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {users.map((userItem) => (
              <TableRow key={userItem.id}>
                <TableCell>
                  <Box display="flex" alignItems="center" gap={1}>
                    <Avatar sx={{ width: 32, height: 32 }}>
                      {userItem.username.charAt(0).toUpperCase()}
                    </Avatar>
                    <Typography variant="body2">
                      {userItem.username}
                    </Typography>
                  </Box>
                </TableCell>
                <TableCell>{userItem.email}</TableCell>
                <TableCell>
                  <Chip
                    icon={getRoleIcon(userItem.role)}
                    label={userItem.role}
                    color={getRoleColor(userItem.role) as any}
                    size="small"
                  />
                </TableCell>
                <TableCell>
                  <Chip
                    label={userItem.isActive ? 'Active' : 'Inactive'}
                    color={userItem.isActive ? 'success' : 'default'}
                    size="small"
                  />
                </TableCell>
                <TableCell>{userItem._count?.nodes || 0}</TableCell>
                <TableCell>
                  <Typography suppressHydrationWarning>
                    {new Date(userItem.createdAt).toLocaleDateString('id-ID')}
                  </Typography>
                </TableCell>
                <TableCell align="right">
                  <IconButton
                    size="small"
                    onClick={(e) => handleOpenMenu(e, userItem)}
                  >
                    <MoreVertIcon />
                  </IconButton>
                </TableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </TableContainer>

      {users.length === 0 && !loading && (
        <Box textAlign="center" py={4}>
          <Typography variant="h6" color="text.secondary" gutterBottom>
            Belum ada user
          </Typography>
          <Typography variant="body2" color="text.secondary" mb={2}>
            Tambahkan user pertama untuk mulai mengelola akses
          </Typography>
          <Button
            variant="contained"
            startIcon={<AddIcon />}
            onClick={() => handleOpenDialog()}
          >
            Tambah User
          </Button>
        </Box>
      )}

      {/* User Form Dialog */}
      <Dialog open={dialogOpen} onClose={handleCloseDialog} maxWidth="sm" fullWidth>
        <DialogTitle>
          {editingUser ? 'Edit User' : 'Tambah User Baru'}
        </DialogTitle>
        <DialogContent>
          <Box component="form" sx={{ mt: 1 }}>
            <TextField
              fullWidth
              label="Email"
              type="email"
              value={formData.email}
              onChange={(e) => setFormData({ ...formData, email: e.target.value })}
              margin="normal"
              required
            />
            <TextField
              fullWidth
              label="Username"
              value={formData.username}
              onChange={(e) => setFormData({ ...formData, username: e.target.value })}
              margin="normal"
              required
            />
            <TextField
              fullWidth
              label={editingUser ? "Password (kosongkan jika tidak ingin mengubah)" : "Password"}
              type="password"
              value={formData.password}
              onChange={(e) => setFormData({ ...formData, password: e.target.value })}
              margin="normal"
              required={!editingUser}
            />
            <FormControl fullWidth margin="normal" required>
              <InputLabel>Role</InputLabel>
              <Select
                value={formData.role}
                onChange={(e) => setFormData({ ...formData, role: e.target.value as any })}
                label="Role"
              >
                <SelectMenuItem value="FREEMIUM">Freemium</SelectMenuItem>
                <SelectMenuItem value="PREMIUM">Premium</SelectMenuItem>
                <SelectMenuItem value="ADMIN">Admin</SelectMenuItem>
              </Select>
            </FormControl>
            <FormControl fullWidth margin="normal">
              <InputLabel>Status</InputLabel>
              <Select
                value={formData.isActive ? 'active' : 'inactive'}
                onChange={(e) => setFormData({ ...formData, isActive: e.target.value === 'active' })}
                label="Status"
              >
                <SelectMenuItem value="active">Active</SelectMenuItem>
                <SelectMenuItem value="inactive">Inactive</SelectMenuItem>
              </Select>
            </FormControl>
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={handleCloseDialog}>Batal</Button>
          <Button
            onClick={handleSubmit}
            variant="contained"
            disabled={submitting || !formData.email || !formData.username || (!editingUser && !formData.password)}
          >
            {submitting ? 'Menyimpan...' : (editingUser ? 'Update' : 'Tambah')}
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
          if (selectedUser) {
            handleOpenDialog(selectedUser)
            handleCloseMenu()
          }
        }}>
          <EditIcon sx={{ mr: 1 }} fontSize="small" />
          Edit
        </MenuItem>
        <MenuItem 
          onClick={() => selectedUser && handleDeleteUser(selectedUser.id)}
          sx={{ color: 'error.main' }}
          disabled={selectedUser?.id === user?.id}
        >
          <DeleteIcon sx={{ mr: 1 }} fontSize="small" />
          Hapus
        </MenuItem>
      </Menu>
    </Box>
  )
}