# Growtopia Bot HTTP Server API

Server HTTP ini menyediakan REST API untuk mengelola dan mengontrol bot Growtopia. API ini memungkinkan Anda untuk menambah, menghapus, dan mengontrol bot melalui HTTP requests.

## Fitur Utama

### Bot Management
- **Add Bot**: Menambahkan bot baru ke pool
- **Remove Bot**: Menghapus bot dari pool
- **List Clients**: Mendapatkan daftar semua bot aktif
- **Disconnect/Reconnect**: Mengontrol koneksi bot

### Bot Control
- **Movement**: Move, teleport bot ke koordinat tertentu
- **World Actions**: Warp ke world lain
- **Block Interaction**: Place, punch, wrench block
- **Item Management**: Consume, wear, collect items
- **Communication**: Send message, smoke

### Information Retrieval
- **Bot Status**: Status koneksi dan kondisi bot
- **Player Info**: Informasi detail player (level, gems, posisi)
- **World Info**: Informasi world (nama, ukuran)
- **Inventory**: Daftar item dalam inventory
- **Ping**: Latency koneksi bot

## Quick Start

1. **Start Server**: Jalankan aplikasi Growtopia Bot
2. **Server URL**: `http://localhost:8080/api`
3. **Method**: POST untuk semua endpoint
4. **Content-Type**: `application/json`

## Contoh Penggunaan

### Menambah Bot Baru
```bash
curl -X POST http://localhost:8080/api \
  -H "Content-Type: application/json" \
  -d '{
    "option": "add",
    "tank_id_name": "mybotname",
    "tank_id_pass": "mypassword",
    "server": "growtopia1.com",
    "port": 17091
  }'
```

### Melihat Semua Bot
```bash
curl -X POST http://localhost:8080/api \
  -H "Content-Type: application/json" \
  -d '{"option": "list_clients"}'
```

### Menggerakkan Bot
```bash
curl -X POST http://localhost:8080/api \
  -H "Content-Type: application/json" \
  -d '{
    "option": "move",
    "tank_id_name": "mybotname",
    "x": 100,
    "y": 200
  }'
```

### Melihat Inventory
```bash
curl -X POST http://localhost:8080/api \
  -H "Content-Type: application/json" \
  -d '{
    "option": "inventory",
    "tank_id_name": "mybotname"
  }'
```

## Response Format

Semua response menggunakan format JSON:

```json
{
  "success": true,
  "message": "Operation completed successfully",
  "data": {
    // Additional response data
  }
}
```

## Error Handling

Jika terjadi error, response akan berformat:

```json
{
  "success": false,
  "error": "Error description"
}
```

## Dokumentasi Lengkap

Untuk dokumentasi API lengkap dengan semua endpoint dan parameter, lihat file `API_DOCUMENTATION.md`.

## Security Notes

- Server berjalan di localhost (127.0.0.1) secara default
- Pastikan firewall dikonfigurasi dengan benar jika mengakses dari jaringan
- Jangan expose server ke internet tanpa autentikasi yang proper

## Troubleshooting

### Server Tidak Bisa Diakses
- Pastikan aplikasi Growtopia Bot sudah berjalan
- Check apakah port 8080 tidak digunakan aplikasi lain
- Verify firewall settings

### Bot Tidak Merespon
- Check status bot dengan endpoint `status`
- Verify bot masih terkoneksi dengan `list_clients`
- Restart bot jika diperlukan dengan `disconnect` dan `reconnect`

### Request Gagal
- Pastikan Content-Type header adalah `application/json`
- Verify JSON format request
- Check parameter yang required sudah lengkap