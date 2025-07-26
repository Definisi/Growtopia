'use client'

import { createContext, useContext, useEffect, useState, ReactNode } from 'react'
import { useRouter } from 'next/navigation'
import axios from 'axios'

interface User {
  id: string
  email: string
  username: string
  role: 'ADMIN' | 'PREMIUM' | 'FREEMIUM'
}

interface AuthContextType {
  user: User | null
  loading: boolean
  login: (email: string, password: string) => Promise<void>
  register: (email: string, username: string, password: string) => Promise<void>
  logout: () => Promise<void>
  refreshUser: () => Promise<void>
}

const AuthContext = createContext<AuthContextType | undefined>(undefined)

export function useAuth() {
  const context = useContext(AuthContext)
  if (context === undefined) {
    throw new Error('useAuth must be used within an AuthProvider')
  }
  return context
}

interface AuthProviderProps {
  children: ReactNode
}

export function AuthProvider({ children }: AuthProviderProps) {
  const [user, setUser] = useState<User | null>(null)
  const [loading, setLoading] = useState(true)
  const [mounted, setMounted] = useState(false)
  const router = useRouter()

  // Check if user is authenticated on mount
  useEffect(() => {
    setMounted(true)
    checkAuth()
  }, [])

  const checkAuth = async () => {
    try {
      // Only access localStorage on the client side
      if (typeof window === 'undefined') {
        setLoading(false)
        return
      }
      
      const token = localStorage.getItem('auth-token')
      if (!token) {
        setLoading(false)
        return
      }

      // Set axios default header
      axios.defaults.headers.common['Authorization'] = `Bearer ${token}`
      
      // Verify token by making a request to protected endpoint
      const response = await axios.get('/api/dashboard/stats')
      if (response.status === 200) {
        // Token is valid, but we need user info
        // For now, we'll decode the token client-side (not recommended for production)
        const payload = JSON.parse(atob(token.split('.')[1]))
        setUser({
          id: payload.userId,
          email: payload.email,
          username: payload.email.split('@')[0], // Fallback
          role: payload.role
        })
      }
    } catch (error) {
      // Token is invalid, remove it
      if (typeof window !== 'undefined') {
        localStorage.removeItem('auth-token')
        delete axios.defaults.headers.common['Authorization']
      }
    } finally {
      setLoading(false)
    }
  }

  const login = async (email: string, password: string) => {
    try {
      const response = await axios.post('/api/auth/login', { email, password })
      const { user: userData, token } = response.data
      
      localStorage.setItem('auth-token', token)
      axios.defaults.headers.common['Authorization'] = `Bearer ${token}`
      setUser(userData)
      
      router.push('/dashboard')
    } catch (error: any) {
      throw new Error(error.response?.data?.error || 'Login gagal')
    }
  }

  const register = async (email: string, username: string, password: string) => {
    try {
      const response = await axios.post('/api/auth/register', { email, username, password })
      const { user: userData, token } = response.data
      
      localStorage.setItem('auth-token', token)
      axios.defaults.headers.common['Authorization'] = `Bearer ${token}`
      setUser(userData)
      
      router.push('/dashboard')
    } catch (error: any) {
      throw new Error(error.response?.data?.error || 'Registrasi gagal')
    }
  }

  const logout = async () => {
    try {
      await axios.post('/api/auth/logout')
    } catch (error) {
      // Ignore logout errors
    } finally {
      localStorage.removeItem('auth-token')
      delete axios.defaults.headers.common['Authorization']
      setUser(null)
      router.push('/login')
    }
  }

  const refreshUser = async () => {
    await checkAuth()
  }

  const value = {
    user,
    loading,
    login,
    register,
    logout,
    refreshUser
  }

  // Only render children after component has mounted on the client
  if (!mounted) {
    return null
  }

  return (
    <AuthContext.Provider value={value}>
      {children}
    </AuthContext.Provider>
  )
}