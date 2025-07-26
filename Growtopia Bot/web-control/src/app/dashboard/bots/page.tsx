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
  Tooltip,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Paper,
  ListItemIcon,
  ListItemText,
  Divider
} from '@mui/material'
import {
  MoreVert as MoreVertIcon,
  PlayArrow as PlayIcon,
  Stop as StopIcon,
  Delete as DeleteIcon,
  SmartToy as BotIcon,
  Computer as NodeIcon,
  Close as CloseIcon,
  Info as InfoIcon,
  Refresh as RefreshIcon,
  ArrowUpward as MoveUpIcon,
  ArrowDownward as MoveDownIcon,
  ArrowBack as MoveLeftIcon,
  ArrowForward as MoveRightIcon,
  DoorFront as DoorIcon,
  Gamepad as ControlIcon,
  Inventory as InventoryIcon,
  FlightTakeoff as TeleportIcon,
  Public as WarpIcon,
  Build as WrenchIcon,
  LocalDining as ConsumeIcon,
  CheckCircle as CollectIcon,
  Cloud as SmokeIcon,
  RestartAlt as ResetIcon,
  LinkOff as DisconnectIcon,
  Refresh as ReconnectIcon,
  Place as PlaceIcon,
  Send as SendIcon,
  CheckCircle
} from '@mui/icons-material'
import { useAuth } from '@/components/AuthProvider'
import DashboardLayout from '@/components/DashboardLayout'
import axios from 'axios'

interface Bot {
  id: string
  name: string
  username: string
  status: 'ONLINE' | 'OFFLINE' | 'ERROR'
  nodeId: string
  createdAt: string
  updatedAt: string
  world?: string
  position?: { x: number; y: number }
  node: {
    id: string
    name: string
    apiUrl: string
    isActive: boolean
  }
  owner?: {
    username: string
    email: string
  }
  // Real-time data from API
  ping?: number
  gems?: number
  worldLock?: boolean
}

interface Node {
  id: string
  name: string
  apiUrl: string
  isActive: boolean
}

export default function DashboardBotsPage() {
  const [bots, setBots] = useState<Bot[]>([])
  const [nodes, setNodes] = useState<Node[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState('')
  const [success, setSuccess] = useState('')
  const [anchorEl, setAnchorEl] = useState<null | HTMLElement>(null)
  const [selectedBot, setSelectedBot] = useState<Bot | null>(null)
  const [actionLoading, setActionLoading] = useState<string | null>(null)
  const [dialogOpen, setDialogOpen] = useState(false)
  const [newBotName, setNewBotName] = useState('')
  const [botUsername, setBotUsername] = useState('')
  const [botPassword, setBotPassword] = useState('')
  const [selectedNodeId, setSelectedNodeId] = useState('')
  const [submitting, setSubmitting] = useState(false)
  const [mounted, setMounted] = useState(false)
  const [contextMenu, setContextMenu] = useState<{
    mouseX: number;
    mouseY: number;
    bot: Bot | null;
  } | null>(null)
  const [moveControlOpen, setMoveControlOpen] = useState(false)
  const [actionDialogOpen, setActionDialogOpen] = useState(false)
  const [actionType, setActionType] = useState('')
  const [actionParams, setActionParams] = useState<any>({})
  const { user } = useAuth()
  
  // Column visibility state
  const [visibleColumns, setVisibleColumns] = useState({
    node: true,
    botName: true,
    username: true,
    status: true,
    ping: true,
    gems: true,
    world: true,
    worldLock: true,
    position: true
  })
  
  // Header context menu
  const [headerContextMenu, setHeaderContextMenu] = useState<{
    mouseX: number;
    mouseY: number;
  } | null>(null)
  
  // Real-time bot data
  const [botRealTimeData, setBotRealTimeData] = useState<Record<string, any>>({})
  const [realTimeLoading, setRealTimeLoading] = useState(false)
  const [lastRefreshTime, setLastRefreshTime] = useState(0)
  const [lastUpdateTime, setLastUpdateTime] = useState<Date | null>(null)

  const fetchBots = async (retryCount = 0) => {
    try {
      setLoading(true)
      setError('')
      const response = await axios.get('/api/bots', { timeout: 10000 })
      setBots(response.data)
      
      // Fetch real-time data for online bots
      fetchRealTimeData(response.data)
    } catch (err: any) {
      if (retryCount < 2 && (err.code === 'ECONNABORTED' || err.response?.status >= 500)) {
        // Retry for timeout or server errors
        setTimeout(() => fetchBots(retryCount + 1), 1000 * (retryCount + 1))
        return
      }
      setError(err.response?.data?.error || 'Gagal memuat bots')
    } finally {
      setLoading(false)
    }
  }
  
  const fetchRealTimeData = async (botsData: Bot[]) => {
    if (realTimeLoading) return // Prevent multiple concurrent requests
    
    const onlineBots = botsData.filter(bot => bot.status === 'ONLINE')
    
    // If no online bots, don't show loading and clear real-time data
    if (onlineBots.length === 0) {
      setBotRealTimeData({})
      return
    }
    
    setRealTimeLoading(true)
    const realTimeData: Record<string, any> = {}
    
    // Use Promise.allSettled for parallel requests with timeout
    const botPromises = onlineBots.map(async (bot) => {
      try {
        const timeout = 5000 // 5 second timeout
        const axiosConfig = { timeout }
        
        // Fetch all data in parallel for each bot
        const [playerInfoResponse, pingResponse, worldInfoResponse] = await Promise.allSettled([
          axios.post(`${bot.node.apiUrl}`, {
            option: 'player_info',
            tank_id_name: bot.username
          }, axiosConfig),
          axios.post(`${bot.node.apiUrl}`, {
            option: 'ping',
            tank_id_name: bot.username
          }, axiosConfig),
          axios.post(`${bot.node.apiUrl}`, {
            option: 'world_info',
            tank_id_name: bot.username
          }, axiosConfig)
        ])
        
        realTimeData[bot.id] = {
          gems: playerInfoResponse.status === 'fulfilled' ? playerInfoResponse.value.data?.gems || 0 : 0,
          ping: pingResponse.status === 'fulfilled' ? pingResponse.value.data?.ping || 0 : 0,
          worldLock: worldInfoResponse.status === 'fulfilled' ? worldInfoResponse.value.data?.has_world_lock || false : false,
          position: playerInfoResponse.status === 'fulfilled' ? playerInfoResponse.value.data?.position || bot.position : bot.position
        }
      } catch (err) {
        // Fallback data for failed requests
        realTimeData[bot.id] = {
          gems: 0,
          ping: 0,
          worldLock: false,
          position: bot.position
        }
      }
    })
    
    // Wait for all bot data to be fetched
    await Promise.allSettled(botPromises)
    
    setBotRealTimeData(realTimeData)
    setLastUpdateTime(new Date())
    setRealTimeLoading(false)
  }

  const fetchNodes = async () => {
    try {
      const response = await axios.get('/api/nodes')
      setNodes(response.data.filter((node: Node) => node.isActive))
    } catch (err: any) {
      console.error('Gagal memuat nodes:', err)
    }
  }

  useEffect(() => {
    setMounted(true)
    fetchBots()
    fetchNodes()
  }, [])
  
  // Separate useEffect for auto-refresh to avoid infinite loop
  useEffect(() => {
    const onlineBots = bots.filter(bot => bot.status === 'ONLINE')
    if (onlineBots.length === 0) return
    
    // Auto-refresh real-time data every 60 seconds (reduced frequency)
    const interval = setInterval(() => {
      const currentOnlineBots = bots.filter(bot => bot.status === 'ONLINE')
      if (currentOnlineBots.length > 0) {
        fetchRealTimeData(bots)
      }
    }, 60000)
    
    return () => clearInterval(interval)
  }, [bots.filter(bot => bot.status === 'ONLINE').length])

  const handleBotAction = async (botId: string, action: 'start' | 'stop') => {
    try {
      setActionLoading(botId)
      setError('')
      await axios.post(`/api/bots/${botId}/action`, { action })
      fetchBots()
    } catch (err: any) {
      setError(err.response?.data?.error || `Gagal ${action} bot`)
    } finally {
      setActionLoading(null)
    }
  }

  const handleDeleteBot = async (botId: string) => {
    if (!confirm('Apakah Anda yakin ingin menghapus bot ini?')) {
      return
    }

    try {
      setError('')
      await axios.delete(`/api/bots/${botId}`)
      fetchBots()
      handleCloseMenu()
    } catch (err: any) {
      setError(err.response?.data?.error || 'Gagal menghapus bot')
    }
  }

  const handleCreateBot = async () => {
    try {
      setSubmitting(true)
      setError('')
      setSuccess('')
      
      const response = await axios.post('/api/bots/create', {
        name: newBotName,
        nodeId: selectedNodeId,
        username: botUsername,
        password: botPassword
      })
      
      // Tutup dialog
      setDialogOpen(false)
      
      // Reset form
      setNewBotName('')
      setSelectedNodeId('')
      setBotUsername('')
      setBotPassword('')
      
      // Refresh daftar bot
      fetchBots()
      
      // Tampilkan pesan sukses
      const statusMessage = response.data.status || 'Bot berhasil dibuat'
      setSuccess(`${response.data.message}. ${statusMessage}`)
      
      // Otomatis sembunyikan pesan sukses setelah 5 detik
      setTimeout(() => setSuccess(''), 5000)
    } catch (err: any) {
      // Menampilkan pesan error yang lebih informatif
      const errorMessage = err.response?.data?.error || 'Gagal membuat bot'
      const errorDetails = err.response?.data?.details
        ? `\nDetail: ${JSON.stringify(err.response.data.details)}`
        : ''
      
      console.error('Error creating bot:', errorMessage, errorDetails)
      setError(errorMessage)
    } finally {
      setSubmitting(false)
    }
  }

  const handleOpenMenu = (event: React.MouseEvent<HTMLElement>, bot: Bot) => {
    setAnchorEl(event.currentTarget)
    setSelectedBot(bot)
  }
  
  // Dialog untuk menampilkan detail bot
  const [detailDialogOpen, setDetailDialogOpen] = useState(false)
  
  const handleOpenDetail = () => {
    if (selectedBot) {
      setDetailDialogOpen(true)
      setAnchorEl(null)
    }
  }
  
  // Fungsi untuk me-refresh status bot secara manual
  const handleRefreshBotStatus = async () => {
    if (!selectedBot) return
    
    try {
      setActionLoading(selectedBot.id)
      setAnchorEl(null)
      
      // Panggil API untuk me-refresh status bot
      const response = await axios.post(`/api/bots/${selectedBot.id}/refresh-status`)
      
      // Update status bot di state
      setBots(prevBots => prevBots.map(bot => 
        bot.id === selectedBot.id ? { ...bot, status: response.data.status } : bot
      ))
      
      // Tampilkan pesan sukses
      setSuccess(`Status bot ${selectedBot.name} berhasil diperbarui: ${response.data.status}`)
      
      // Otomatis sembunyikan pesan sukses setelah 5 detik
      setTimeout(() => setSuccess(''), 5000)
    } catch (err: any) {
      const errorMessage = err.response?.data?.error || `Gagal memperbarui status bot ${selectedBot.name}`
      setError(errorMessage)
    } finally {
      setActionLoading(null)
    }
  }

  const handleCloseMenu = () => {
    setAnchorEl(null)
    setSelectedBot(null)
  }

  const handleContextMenu = (event: React.MouseEvent, bot: Bot) => {
    event.preventDefault()
    setContextMenu(
      contextMenu === null
        ? {
            mouseX: event.clientX + 2,
            mouseY: event.clientY - 6,
            bot,
          }
        : null,
    )
  }
  
  const handleHeaderContextMenu = (event: React.MouseEvent) => {
    event.preventDefault()
    setHeaderContextMenu({
      mouseX: event.clientX + 2,
      mouseY: event.clientY - 6
    })
  }
  
  const handleCloseHeaderMenu = () => {
    setHeaderContextMenu(null)
  }
  
  const toggleColumn = (column: keyof typeof visibleColumns) => {
    setVisibleColumns(prev => ({
      ...prev,
      [column]: !prev[column]
    }))
    handleCloseHeaderMenu()
  }
  
  const handleManualRefresh = () => {
    const now = Date.now()
    const timeSinceLastRefresh = now - lastRefreshTime
    
    // Prevent refresh if less than 3 seconds since last refresh
    if (timeSinceLastRefresh < 3000) {
      return
    }
    
    setLastRefreshTime(now)
    fetchRealTimeData(bots)
  }

  const handleCloseContextMenu = () => {
    setContextMenu(null)
  }

  const handleBotApiAction = async (action: string, params: any = {}) => {
    if (!contextMenu?.bot) return
    
    try {
      setActionLoading(contextMenu.bot.id)
      setError('')
      
      const response = await axios.post(`${contextMenu.bot.node.apiUrl}/api/1.0`, {
        option: action,
        tank_id_name: contextMenu.bot.username,
        ...params
      })
      
      if (response.data.success) {
        setSuccess(`Aksi ${action} berhasil dijalankan pada bot ${contextMenu.bot.name} (${contextMenu.bot.username})`)
        setTimeout(() => setSuccess(''), 3000)
      } else {
        setError(`Gagal menjalankan aksi ${action} pada bot ${contextMenu.bot.name}: ${response.data.message || 'Unknown error'}`)
      }
    } catch (err: any) {
      setError(err.response?.data?.error || `Gagal menjalankan aksi ${action}`)
    } finally {
      setActionLoading(null)
      setContextMenu(null)
    }
  }

  const handleMoveAction = (direction: string) => {
    if (!contextMenu?.bot) return
    
    const moveParams: any = {}
    
    switch (direction) {
      case 'up':
        moveParams.y = -1
        break
      case 'down':
        moveParams.y = 1
        break
      case 'left':
        moveParams.x = -1
        break
      case 'right':
        moveParams.x = 1
        break
      case 'door':
        // Door action might need specific coordinates
        break
    }
    
    handleBotApiAction('move', moveParams)
    setMoveControlOpen(false)
  }

  const openActionDialog = (action: string, defaultParams: any = {}) => {
    setActionType(action)
    setActionParams(defaultParams)
    setActionDialogOpen(true)
    setContextMenu(null)
  }

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'ONLINE':
        return 'success'
      case 'OFFLINE':
        return 'default'
      case 'ERROR':
        return 'error'
      default:
        return 'default'
    }
  }

  const getStatusIcon = (status: string) => {
    switch (status) {
      case 'ONLINE':
        return <PlayIcon />
      case 'OFFLINE':
        return <StopIcon />
      case 'ERROR':
        return <StopIcon />
      default:
        return <StopIcon />
    }
  }

  if (!mounted || loading) {
    return (
      <DashboardLayout>
        <Box display="flex" flexDirection="column" justifyContent="center" alignItems="center" minHeight="400px" gap={2}>
          <CircularProgress />
          <Typography variant="body2" color="text.secondary">
            {!mounted ? 'Memuat aplikasi...' : 'Memuat data bot...'}
          </Typography>
        </Box>
      </DashboardLayout>
    )
  }

  return (
    <DashboardLayout>
      <Box>
        <Box display="flex" justifyContent="space-between" alignItems="center" mb={3}>
        <Typography variant="h4" component="h1">
          Bot Management
        </Typography>
        <Box display="flex" flexDirection="column" alignItems="flex-end" gap={1}>
          <Box display="flex" gap={2}>
            <Button
              variant="outlined"
              startIcon={realTimeLoading ? <CircularProgress size={16} /> : <RefreshIcon />}
              onClick={handleManualRefresh}
              disabled={loading || bots.length === 0 || realTimeLoading}
            >
              {realTimeLoading ? 'Refreshing...' : 'Refresh Data'}
            </Button>
            <Button
              variant="contained"
              startIcon={<BotIcon />}
              onClick={() => setDialogOpen(true)}
              disabled={nodes.length === 0}
            >
              Tambah Bot
            </Button>
          </Box>
          {lastUpdateTime && (
            <Typography variant="caption" color="text.secondary">
              Terakhir diperbarui: {lastUpdateTime.toLocaleTimeString('id-ID')}
            </Typography>
          )}
        </Box>
      </Box>

      {error && (
        <Alert 
          severity="error" 
          sx={{ mb: 2 }}
          action={
            <IconButton
              aria-label="close"
              color="inherit"
              size="small"
              onClick={() => setError('')}
            >
              <CloseIcon fontSize="inherit" />
            </IconButton>
          }
        >
          {error}
        </Alert>
      )}
      
      {success && (
        <Alert 
          severity="success" 
          sx={{ mb: 2 }}
          action={
            <IconButton
              aria-label="close"
              color="inherit"
              size="small"
              onClick={() => setSuccess('')}
            >
              <CloseIcon fontSize="inherit" />
            </IconButton>
          }
        >
          {success}
        </Alert>
      )}

      {nodes.length === 0 && (
        <Alert severity="warning" sx={{ mb: 2 }}>
          Tidak ada node aktif. Silakan tambahkan dan aktifkan node terlebih dahulu.
        </Alert>
      )}

      <TableContainer component={Paper}>
        <Table>
          <TableHead>
            <Tooltip title="Klik kanan untuk mengatur kolom yang ditampilkan" placement="top">
              <TableRow onContextMenu={handleHeaderContextMenu} sx={{ cursor: 'context-menu' }}>
                {visibleColumns.node && <TableCell>Node</TableCell>}
                {visibleColumns.botName && <TableCell>Bot Name</TableCell>}
                {visibleColumns.username && <TableCell>Username</TableCell>}
                {visibleColumns.status && <TableCell>Status</TableCell>}
                {visibleColumns.ping && <TableCell>Ping (ms)</TableCell>}
                {visibleColumns.gems && <TableCell>Gems</TableCell>}
                {visibleColumns.world && <TableCell>World</TableCell>}
                {visibleColumns.worldLock && <TableCell>World Lock</TableCell>}
                {visibleColumns.position && <TableCell>Position (X, Y)</TableCell>}
                <TableCell>Actions</TableCell>
              </TableRow>
            </Tooltip>
          </TableHead>
          <TableBody>
            {bots.map((bot) => {
              const realTimeData = botRealTimeData[bot.id] || {}
              return (
                <TableRow
                  key={bot.id}
                  onContextMenu={(e) => handleContextMenu(e, bot)}
                  sx={{ 
                    '&:hover': { backgroundColor: 'action.hover' },
                    cursor: 'context-menu'
                  }}
                >
                  {visibleColumns.node && (
                    <TableCell>
                      <Box display="flex" alignItems="center" gap={1}>
                        <NodeIcon fontSize="small" color="action" />
                        <Typography variant="body2">
                          {bot.node.name}
                        </Typography>
                      </Box>
                    </TableCell>
                  )}
                  {visibleColumns.botName && (
                    <TableCell>
                      <Box display="flex" alignItems="center" gap={1}>
                        <BotIcon color="primary" fontSize="small" />
                        <Typography variant="body2" fontWeight="medium">
                          {bot.name}
                        </Typography>
                      </Box>
                    </TableCell>
                  )}
                  {visibleColumns.username && (
                    <TableCell>
                      <Typography variant="body2" color="text.secondary">
                        {bot.username || '-'}
                      </Typography>
                    </TableCell>
                  )}
                  {visibleColumns.status && (
                    <TableCell>
                      <Tooltip title={bot.status === 'ONLINE' ? 'Bot terhubung ke server Growtopia' : 'Bot tidak terhubung ke server Growtopia'}>
                        <Chip
                          icon={getStatusIcon(bot.status)}
                          label={bot.status}
                          color={getStatusColor(bot.status) as any}
                          size="small"
                        />
                      </Tooltip>
                    </TableCell>
                  )}
                  {visibleColumns.ping && (
                    <TableCell>
                      <Box display="flex" alignItems="center" gap={1}>
                        {realTimeLoading && bot.status === 'ONLINE' && (
                          <CircularProgress size={12} />
                        )}
                        <Typography variant="body2" color={bot.status === 'ONLINE' ? 'text.primary' : 'text.disabled'}>
                          {bot.status === 'ONLINE' ? `${realTimeData.ping || 0}ms` : '-'}
                        </Typography>
                      </Box>
                    </TableCell>
                  )}
                  {visibleColumns.gems && (
                    <TableCell>
                      <Box display="flex" alignItems="center" gap={1}>
                        {realTimeLoading && bot.status === 'ONLINE' && (
                          <CircularProgress size={12} />
                        )}
                        <Typography variant="body2" color={bot.status === 'ONLINE' ? 'text.primary' : 'text.disabled'}>
                          {bot.status === 'ONLINE' ? (realTimeData.gems || 0).toLocaleString() : '-'}
                        </Typography>
                      </Box>
                    </TableCell>
                  )}
                  {visibleColumns.world && (
                    <TableCell>
                      <Typography variant="body2" color="text.secondary">
                        {bot.world || '-'}
                      </Typography>
                    </TableCell>
                  )}
                  {visibleColumns.worldLock && (
                    <TableCell>
                      <Chip
                        label={realTimeData.worldLock ? 'Yes' : 'No'}
                        color={realTimeData.worldLock ? 'success' : 'default'}
                        size="small"
                        variant="outlined"
                      />
                    </TableCell>
                  )}
                  {visibleColumns.position && (
                    <TableCell>
                      <Typography variant="body2" color="text.secondary">
                        {realTimeData.position ? 
                          `${realTimeData.position.x}, ${realTimeData.position.y}` : 
                          (bot.position ? `${bot.position.x}, ${bot.position.y}` : '-')
                        }
                      </Typography>
                    </TableCell>
                  )}
                  <TableCell>
                    <IconButton
                      size="small"
                      onClick={(e) => handleOpenMenu(e, bot)}
                      disabled={actionLoading === bot.id}
                    >
                      {actionLoading === bot.id ? (
                        <CircularProgress size={16} />
                      ) : (
                        <MoreVertIcon />
                      )}
                    </IconButton>
                  </TableCell>
                </TableRow>
              )
            })}
          </TableBody>
        </Table>
      </TableContainer>

      {bots.length === 0 && !loading && (
        <Box textAlign="center" py={4}>
          <Typography variant="h6" color="text.secondary" gutterBottom>
            Belum ada bot
          </Typography>
          <Typography variant="body2" color="text.secondary" mb={2}>
            Tambahkan bot pertama Anda untuk mulai automasi
          </Typography>
          <Button
            variant="contained"
            startIcon={<BotIcon />}
            onClick={() => setDialogOpen(true)}
            disabled={nodes.length === 0}
          >
            Tambah Bot
          </Button>
        </Box>
      )}

      {/* Header Context Menu for Column Visibility */}
      <Menu
        open={headerContextMenu !== null}
        onClose={handleCloseHeaderMenu}
        anchorReference="anchorPosition"
        anchorPosition={
          headerContextMenu !== null
            ? { top: headerContextMenu.mouseY, left: headerContextMenu.mouseX }
            : undefined
        }
      >
        <MenuItem onClick={() => toggleColumn('node')}>
          <ListItemIcon>
            {visibleColumns.node ? <CheckCircle fontSize="small" /> : <div style={{ width: 20 }} />}
          </ListItemIcon>
          <ListItemText>Node</ListItemText>
        </MenuItem>
        <MenuItem onClick={() => toggleColumn('botName')}>
          <ListItemIcon>
            {visibleColumns.botName ? <CheckCircle fontSize="small" /> : <div style={{ width: 20 }} />}
          </ListItemIcon>
          <ListItemText>Bot Name</ListItemText>
        </MenuItem>
        <MenuItem onClick={() => toggleColumn('username')}>
          <ListItemIcon>
            {visibleColumns.username ? <CheckCircle fontSize="small" /> : <div style={{ width: 20 }} />}
          </ListItemIcon>
          <ListItemText>Username</ListItemText>
        </MenuItem>
        <MenuItem onClick={() => toggleColumn('status')}>
          <ListItemIcon>
            {visibleColumns.status ? <CheckCircle fontSize="small" /> : <div style={{ width: 20 }} />}
          </ListItemIcon>
          <ListItemText>Status</ListItemText>
        </MenuItem>
        <MenuItem onClick={() => toggleColumn('ping')}>
          <ListItemIcon>
            {visibleColumns.ping ? <CheckCircle fontSize="small" /> : <div style={{ width: 20 }} />}
          </ListItemIcon>
          <ListItemText>Ping</ListItemText>
        </MenuItem>
        <MenuItem onClick={() => toggleColumn('gems')}>
          <ListItemIcon>
            {visibleColumns.gems ? <CheckCircle fontSize="small" /> : <div style={{ width: 20 }} />}
          </ListItemIcon>
          <ListItemText>Gems</ListItemText>
        </MenuItem>
        <MenuItem onClick={() => toggleColumn('world')}>
          <ListItemIcon>
            {visibleColumns.world ? <CheckCircle fontSize="small" /> : <div style={{ width: 20 }} />}
          </ListItemIcon>
          <ListItemText>World</ListItemText>
        </MenuItem>
        <MenuItem onClick={() => toggleColumn('worldLock')}>
          <ListItemIcon>
            {visibleColumns.worldLock ? <CheckCircle fontSize="small" /> : <div style={{ width: 20 }} />}
          </ListItemIcon>
          <ListItemText>World Lock</ListItemText>
        </MenuItem>
        <MenuItem onClick={() => toggleColumn('position')}>
          <ListItemIcon>
            {visibleColumns.position ? <CheckCircle fontSize="small" /> : <div style={{ width: 20 }} />}
          </ListItemIcon>
          <ListItemText>Position</ListItemText>
        </MenuItem>
      </Menu>

      {/* Detail Bot Dialog */}
      <Dialog open={detailDialogOpen} onClose={() => setDetailDialogOpen(false)} maxWidth="sm" fullWidth>
        <DialogTitle>
          Detail Bot: {selectedBot?.name}
          <IconButton
            aria-label="close"
            onClick={() => setDetailDialogOpen(false)}
            sx={{ position: 'absolute', right: 8, top: 8 }}
          >
            <CloseIcon />
          </IconButton>
        </DialogTitle>
        <DialogContent dividers>
          {selectedBot && (
            <Box>
              <Box sx={{ mb: 3 }}>
                <Typography variant="subtitle1" gutterBottom>Status</Typography>
                <Box display="flex" alignItems="center">
                  <Chip
                    icon={getStatusIcon(selectedBot.status)}
                    label={selectedBot.status}
                    color={getStatusColor(selectedBot.status) as any}
                    size="small"
                  />
                  <Typography variant="body2" color="text.secondary" sx={{ ml: 1 }}>
                    {selectedBot.status === 'ONLINE' ? 'Bot terhubung ke server Growtopia' : 'Bot tidak terhubung ke server Growtopia'}
                  </Typography>
                </Box>
              </Box>
              
              <Box sx={{ mb: 3 }}>
                <Typography variant="subtitle1" gutterBottom>Informasi Bot</Typography>
                <Grid container spacing={2}>
                  <Grid size={4}>
                    <Typography variant="body2" color="text.secondary">ID Bot</Typography>
                  </Grid>
                  <Grid size={8}>
                    <Typography variant="body2">{selectedBot.id}</Typography>
                  </Grid>
                  
                  <Grid size={4}>
                    <Typography variant="body2" color="text.secondary">Nama Bot</Typography>
                  </Grid>
                  <Grid size={8}>
                    <Typography variant="body2">{selectedBot.name}</Typography>
                  </Grid>
                  
                  <Grid size={4}>
                    <Typography variant="body2" color="text.secondary">Username Bot</Typography>
                  </Grid>
                  <Grid size={8}>
                    <Typography variant="body2">{selectedBot.username || '-'}</Typography>
                  </Grid>
                  
                  <Grid size={4}>
                    <Typography variant="body2" color="text.secondary">Dibuat pada</Typography>
                  </Grid>
                  <Grid size={8}>
                    <Typography variant="body2">{new Date(selectedBot.createdAt).toLocaleString('id-ID')}</Typography>
                  </Grid>
                  
                  <Grid size={4}>
                    <Typography variant="body2" color="text.secondary">Terakhir diperbarui</Typography>
                  </Grid>
                  <Grid size={8}>
                    <Typography variant="body2">{new Date(selectedBot.updatedAt).toLocaleString('id-ID')}</Typography>
                  </Grid>
                </Grid>
              </Box>
              
              <Box sx={{ mb: 3 }}>
                <Typography variant="subtitle1" gutterBottom>Informasi Node</Typography>
                <Grid container spacing={2}>
                  <Grid size={4}>
                    <Typography variant="body2" color="text.secondary">Nama Node</Typography>
                  </Grid>
                  <Grid size={8}>
                    <Typography variant="body2">{selectedBot.node.name}</Typography>
                  </Grid>
                  
                  <Grid size={4}>
                    <Typography variant="body2" color="text.secondary">API URL</Typography>
                  </Grid>
                  <Grid size={8}>
                    <Typography variant="body2">{selectedBot.node.apiUrl}</Typography>
                  </Grid>
                  
                  <Grid size={4}>
                    <Typography variant="body2" color="text.secondary">Status Node</Typography>
                  </Grid>
                  <Grid size={8}>
                    <Chip 
                      size="small" 
                      color={selectedBot.node.isActive ? "success" : "error"}
                      label={selectedBot.node.isActive ? "Aktif" : "Tidak Aktif"}
                    />
                  </Grid>
                </Grid>
              </Box>
              
              {selectedBot.owner && (
                <Box>
                  <Typography variant="subtitle1" gutterBottom>Informasi Pemilik</Typography>
                  <Grid container spacing={2}>
                    <Grid size={4}>
                      <Typography variant="body2" color="text.secondary">Username</Typography>
                    </Grid>
                    <Grid size={8}>
                      <Typography variant="body2">{selectedBot.owner.username}</Typography>
                    </Grid>
                    
                    <Grid size={4}>
                      <Typography variant="body2" color="text.secondary">Email</Typography>
                    </Grid>
                    <Grid size={8}>
                      <Typography variant="body2">{selectedBot.owner.email}</Typography>
                    </Grid>
                  </Grid>
                </Box>
              )}
            </Box>
          )}
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setDetailDialogOpen(false)}>Tutup</Button>
        </DialogActions>
      </Dialog>
      
      {/* Create Bot Dialog */}
      <Dialog open={dialogOpen} onClose={() => setDialogOpen(false)} maxWidth="sm" fullWidth>
        <DialogTitle>Tambah Bot Baru</DialogTitle>
        <DialogContent>
          {error && (
            <Alert 
              severity="error" 
              sx={{ mt: 2, mb: 1 }}
              action={
                <IconButton
                  aria-label="close"
                  color="inherit"
                  size="small"
                  onClick={() => setError('')}
                >
                  <CloseIcon fontSize="inherit" />
                </IconButton>
              }
            >
              {error}
            </Alert>
          )}
          <Box component="form" sx={{ mt: 1 }}>
            <TextField
              fullWidth
              label="Nama Bot"
              value={newBotName}
              onChange={(e) => setNewBotName(e.target.value)}
              margin="normal"
              required
              helperText="Nama untuk ditampilkan di dashboard"
            />
            <TextField
              fullWidth
              label="Username Bot"
              value={botUsername}
              onChange={(e) => setBotUsername(e.target.value)}
              margin="normal"
              required
              helperText="Username untuk login ke Growtopia"
            />
            <TextField
              fullWidth
              label="Password Bot"
              type="password"
              value={botPassword}
              onChange={(e) => setBotPassword(e.target.value)}
              margin="normal"
              required
              helperText="Password untuk login ke Growtopia"
            />
            <FormControl fullWidth margin="normal" required>
              <InputLabel>Pilih Node</InputLabel>
              <Select
                value={selectedNodeId}
                onChange={(e) => setSelectedNodeId(e.target.value)}
                label="Pilih Node"
              >
                {nodes.map((node) => (
                  <SelectMenuItem 
                    key={node.id} 
                    value={node.id}
                    sx={{
                      display: 'flex',
                      flexDirection: 'column',
                      alignItems: 'flex-start'
                    }}
                  >
                    <Box sx={{ display: 'flex', alignItems: 'center', width: '100%' }}>
                      <Typography variant="body1">{node.name}</Typography>
                      <Chip 
                        size="small" 
                        color={node.isActive ? "success" : "error"}
                        label={node.isActive ? "Aktif" : "Tidak Aktif"}
                        sx={{ ml: 1, height: 20 }}
                      />
                    </Box>
                    <Typography variant="caption" color="text.secondary">
                      {node.apiUrl}
                    </Typography>
                  </SelectMenuItem>
                ))}
              </Select>
              <Typography variant="caption" color="text.secondary" sx={{ mt: 0.5 }}>
                Pilih node tempat bot akan dijalankan
              </Typography>
            </FormControl>
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setDialogOpen(false)}>Batal</Button>
          <Button
            onClick={handleCreateBot}
            variant="contained"
            disabled={submitting || !newBotName || !botUsername || !botPassword || !selectedNodeId}
            startIcon={submitting ? <CircularProgress size={20} color="inherit" /> : null}
          >
            {submitting ? 'Membuat...' : 'Tambah'}
          </Button>
        </DialogActions>
      </Dialog>

      {/* Context Menu */}
      <Menu
        anchorEl={anchorEl}
        open={Boolean(anchorEl)}
        onClose={handleCloseMenu}
      >
        <MenuItem onClick={handleOpenDetail}>
          <InfoIcon sx={{ mr: 1 }} fontSize="small" />
          Lihat Detail
        </MenuItem>
        <MenuItem onClick={handleRefreshBotStatus}>
          <RefreshIcon sx={{ mr: 1 }} fontSize="small" />
          Refresh Status
        </MenuItem>
        {selectedBot?.status === 'OFFLINE' && (
          <MenuItem onClick={() => {
            if (selectedBot) {
              handleBotAction(selectedBot.id, 'start')
              handleCloseMenu()
            }
          }}>
            <PlayIcon sx={{ mr: 1 }} fontSize="small" />
            Start Bot
          </MenuItem>
        )}
        {selectedBot?.status === 'ONLINE' && (
          <MenuItem onClick={() => {
            if (selectedBot) {
              handleBotAction(selectedBot.id, 'stop')
              handleCloseMenu()
            }
          }}>
            <StopIcon sx={{ mr: 1 }} fontSize="small" />
            Stop Bot
          </MenuItem>
        )}
        <MenuItem 
          onClick={() => selectedBot && handleDeleteBot(selectedBot.id)}
          sx={{ color: 'error.main' }}
        >
          <DeleteIcon sx={{ mr: 1 }} fontSize="small" />
          Hapus
        </MenuItem>
      </Menu>

      {/* Bot Action Context Menu */}
      <Menu
        open={contextMenu !== null}
        onClose={handleCloseContextMenu}
        anchorReference="anchorPosition"
        anchorPosition={
          contextMenu !== null
            ? { top: contextMenu.mouseY, left: contextMenu.mouseX }
            : undefined
        }
      >
        <MenuItem onClick={() => setMoveControlOpen(true)}>
          <ListItemIcon>
            <ControlIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Move Control</ListItemText>
        </MenuItem>
        
        <Divider />
        
        <MenuItem onClick={() => handleBotApiAction('inventory')}>
          <ListItemIcon>
            <InventoryIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>View Inventory</ListItemText>
        </MenuItem>
        
        <MenuItem onClick={() => openActionDialog('teleport')}>
          <ListItemIcon>
            <TeleportIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Teleport</ListItemText>
        </MenuItem>
        
        <MenuItem onClick={() => openActionDialog('warp')}>
          <ListItemIcon>
            <WarpIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Warp to World</ListItemText>
        </MenuItem>
        
        <MenuItem onClick={() => openActionDialog('place')}>
          <ListItemIcon>
            <PlaceIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Place Item</ListItemText>
        </MenuItem>
        
        <MenuItem onClick={() => openActionDialog('wrench')}>
          <ListItemIcon>
            <WrenchIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Wrench</ListItemText>
        </MenuItem>
        
        <MenuItem onClick={() => openActionDialog('consume')}>
          <ListItemIcon>
            <ConsumeIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Consume Item</ListItemText>
        </MenuItem>
        
        <MenuItem onClick={() => handleBotApiAction('collect')}>
          <ListItemIcon>
            <CollectIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Collect Items</ListItemText>
        </MenuItem>
        
        <Divider />
        
        <MenuItem onClick={() => handleBotApiAction('smoke')}>
          <ListItemIcon>
            <SmokeIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Smoke</ListItemText>
        </MenuItem>
        
        <MenuItem onClick={() => handleBotApiAction('reset')}>
          <ListItemIcon>
            <ResetIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Reset Bot</ListItemText>
        </MenuItem>
        
        <Divider />
        
        <MenuItem onClick={() => handleBotApiAction('disconnect')}>
          <ListItemIcon>
            <DisconnectIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Disconnect</ListItemText>
        </MenuItem>
        
        <MenuItem onClick={() => handleBotApiAction('reconnect')}>
          <ListItemIcon>
            <ReconnectIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Reconnect</ListItemText>
        </MenuItem>
      </Menu>

      {/* Move Control Dialog */}
      <Dialog open={moveControlOpen} onClose={() => setMoveControlOpen(false)}>
        <DialogTitle>Move Control - {contextMenu?.bot?.name}</DialogTitle>
        <DialogContent>
          <Box display="flex" flexDirection="column" alignItems="center" gap={2} p={2}>
            <Button
              variant="outlined"
              startIcon={<MoveUpIcon />}
              onClick={() => handleMoveAction('up')}
              fullWidth
            >
              Move Up
            </Button>
            
            <Box display="flex" gap={2}>
              <Button
                variant="outlined"
                startIcon={<MoveLeftIcon />}
                onClick={() => handleMoveAction('left')}
              >
                Move Left
              </Button>
              
              <Button
                variant="outlined"
                startIcon={<MoveRightIcon />}
                onClick={() => handleMoveAction('right')}
              >
                Move Right
              </Button>
            </Box>
            
            <Button
              variant="outlined"
              startIcon={<MoveDownIcon />}
              onClick={() => handleMoveAction('down')}
              fullWidth
            >
              Move Down
            </Button>
            
            <Divider sx={{ width: '100%', my: 1 }} />
            
            <Button
              variant="contained"
              startIcon={<DoorIcon />}
              onClick={() => handleMoveAction('door')}
              fullWidth
              color="primary"
            >
              Enter Door
            </Button>
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setMoveControlOpen(false)}>Close</Button>
        </DialogActions>
      </Dialog>

      {/* Action Dialog */}
      <Dialog open={actionDialogOpen} onClose={() => setActionDialogOpen(false)}>
        <DialogTitle>Bot Action - {actionType}</DialogTitle>
        <DialogContent>
          <Box display="flex" flexDirection="column" gap={2} pt={1}>
            {actionType === 'teleport' && (
              <>
                <TextField
                  label="X Position"
                  type="number"
                  value={actionParams.x || ''}
                  onChange={(e) => setActionParams({...actionParams, x: parseInt(e.target.value) || 0})}
                  fullWidth
                />
                <TextField
                  label="Y Position"
                  type="number"
                  value={actionParams.y || ''}
                  onChange={(e) => setActionParams({...actionParams, y: parseInt(e.target.value) || 0})}
                  fullWidth
                />
              </>
            )}
            
            {actionType === 'warp' && (
              <TextField
                label="World Name"
                value={actionParams.name || ''}
                onChange={(e) => setActionParams({...actionParams, name: e.target.value})}
                fullWidth
              />
            )}
            
            {actionType === 'place' && (
              <>
                <TextField
                  label="X Position"
                  type="number"
                  value={actionParams.x || ''}
                  onChange={(e) => setActionParams({...actionParams, x: parseInt(e.target.value) || 0})}
                  fullWidth
                />
                <TextField
                  label="Y Position"
                  type="number"
                  value={actionParams.y || ''}
                  onChange={(e) => setActionParams({...actionParams, y: parseInt(e.target.value) || 0})}
                  fullWidth
                />
                <TextField
                  label="Item ID"
                  type="number"
                  value={actionParams.item_id || ''}
                  onChange={(e) => setActionParams({...actionParams, item_id: parseInt(e.target.value) || 0})}
                  fullWidth
                />
              </>
            )}
            
            {actionType === 'wrench' && (
              <>
                <TextField
                  label="X Position"
                  type="number"
                  value={actionParams.x || ''}
                  onChange={(e) => setActionParams({...actionParams, x: parseInt(e.target.value) || 0})}
                  fullWidth
                />
                <TextField
                  label="Y Position"
                  type="number"
                  value={actionParams.y || ''}
                  onChange={(e) => setActionParams({...actionParams, y: parseInt(e.target.value) || 0})}
                  fullWidth
                />
              </>
            )}
            
            {actionType === 'consume' && (
              <TextField
                label="Item ID"
                type="number"
                value={actionParams.item_id || ''}
                onChange={(e) => setActionParams({...actionParams, item_id: parseInt(e.target.value) || 0})}
                fullWidth
              />
            )}
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setActionDialogOpen(false)}>Cancel</Button>
          <Button 
            onClick={() => {
              handleBotApiAction(actionType, actionParams)
              setActionDialogOpen(false)
              setActionParams({})
            }}
            variant="contained"
          >
            Execute
          </Button>
        </DialogActions>
      </Dialog>
    </Box>
    </DashboardLayout>
  )
}