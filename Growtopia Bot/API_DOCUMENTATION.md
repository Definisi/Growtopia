# Growtopia Bot HTTP API Documentation

## Overview
Growtopia Bot sekarang berfokus pada HTTP API untuk mengelola bot tanpa GUI. Server HTTP berjalan di `http://localhost:8080` dengan endpoint utama di `/api/1.0`.

## API Endpoints
**Base URL:** `http://localhost:8080`

### GET /ok
Health check endpoint to verify server status.

**Response:**
```json
{
  "status": "OK",
  "message": "Server is running",
  "timestamp": 1234567890
}
```

**Example:**
```bash
curl -X GET http://localhost:8080/ok
```

### POST /api/1.0
**Content-Type:** `application/json`

## Available Operations

### 1. Add Bot
Menambahkan bot baru ke pool.

```json
{
  "option": "add",
  "tank_id_name": "username",
  "tank_id_pass": "password",
  "socks5": {
    "ip": "127.0.0.1",
    "port": 1080,
    "username": "proxy_user",
    "password": "proxy_pass"
  }
}
```

### 2. Remove Bot
Menghapus bot dari pool.

```json
{
  "option": "remove",
  "tank_id_name": "username"
}
```

### 3. Get Client Info
Mendapatkan informasi bot (posisi, world, dll).

```json
{
  "option": "client",
  "tank_id_name": "username"
}
```

### 4. Consume Item
Menggunakan item dari inventory.

```json
{
  "option": "consume",
  "tank_id_name": "username",
  "item_id": 123
}
```

### 5. Move Bot
Memindahkan bot ke koordinat tertentu.

```json
{
  "option": "move",
  "tank_id_name": "username",
  "x": 100,
  "y": 200
}
```

### 6. Place Block
Menempatkan block di koordinat tertentu.

```json
{
  "option": "place",
  "tank_id_name": "username",
  "x": 100,
  "y": 200,
  "item_id": 2
}
```

### 7. Send Packet
Mengirim packet custom.

```json
{
  "option": "send",
  "tank_id_name": "username",
  "type": 2,
  "packet": "packet_data"
}
```

### 8. Warp to World
Warp bot ke world tertentu.

```json
{
  "option": "warp",
  "tank_id_name": "username",
  "name": "WORLDNAME"
}
```

### 9. Wrench Block
Menggunakan wrench pada block.

```json
{
  "option": "wrench",
  "tank_id_name": "username",
  "x": 100,
  "y": 200
}
```

## Response Format
Semua response menggunakan format JSON:

```json
{
  "success": true/false,
  "info": {
    // Additional data (untuk operasi client info)
    "x": 100,
    "y": 200,
    "world": "WORLDNAME"
  }
}
```

## Example Usage with cURL

```bash
# Add a bot
curl -X POST http://localhost:8080/api/1.0 \
  -H "Content-Type: application/json" \
  -d '{
    "option": "add",
    "tank_id_name": "mybot",
    "tank_id_pass": "mypassword"
  }'

# Move bot
curl -X POST http://localhost:8080/api/1.0 \
  -H "Content-Type: application/json" \
  -d '{
    "option": "move",
    "tank_id_name": "mybot",
    "x": 100,
    "y": 200
  }'

# Get bot info
curl -X POST http://localhost:8080/api/1.0 \
  -H "Content-Type: application/json" \
  -d '{
    "option": "client",
    "tank_id_name": "mybot"
  }'
```

## Running the Server
1. Compile the project dengan Visual Studio atau MSBuild
2. Jalankan executable yang dihasilkan
3. Server akan start di `http://localhost:8080`
4. Gunakan API endpoints di atas untuk mengelola bot

## Notes
- Semua operasi ImGui telah dihapus
- Project sekarang fokus pada HTTP API saja
- Server berjalan dalam console mode
- Gunakan tools seperti Postman, cURL, atau aplikasi HTTP client lainnya untuk berinteraksi dengan API