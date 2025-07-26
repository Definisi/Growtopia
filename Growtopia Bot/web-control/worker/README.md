# Growtopia Bot Worker

Worker service untuk melakukan sinkronisasi bot antara database dan node-node yang aktif.

## Fungsi Utama

Worker ini bertugas untuk:

1. **Cek Status Node**: Melakukan health check ke semua node yang terdaftar di database
2. **Sinkronisasi Bot**: 
   - Jika node tidak aktif → hapus semua bot dari node tersebut di database
   - Jika node aktif tapi tidak ada bot → hapus bot yang ada di database untuk node tersebut
   - Jika node aktif dan ada bot → update status bot di database

## Instalasi

```bash
cd worker
npm install
```

## Konfigurasi

Edit file `.env` untuk mengatur konfigurasi:

```env
DATABASE_URL="file:../prisma/dev.db"
SYNC_INTERVAL_MINUTES=5
NODE_TIMEOUT_MS=5000
BOT_API_TIMEOUT_MS=10000
```

## Menjalankan Worker

### Development
```bash
npm run dev
```

### Production
```bash
npm start
```

## Cara Kerja

1. **Scheduler**: Worker berjalan setiap 5 menit (dapat dikonfigurasi)
2. **Health Check**: Melakukan GET request ke `http://node:port/health`
3. **Bot Check**: Melakukan GET request ke `http://node:port/api/bots`
4. **Database Sync**: 
   - Hapus bot yang tidak aktif
   - Update/tambah bot yang aktif

## API Endpoints yang Digunakan

### Node Health Check
```
GET http://node:port/health
Response: 200 OK (jika node aktif)
```

### Get Active Bots
```
GET http://node:port/api/bots
Response: {
  "bots": [
    {
      "username": "bot1",
      "status": "ONLINE",
      "tankIdName": "bot1"
    }
  ]
}
```

## Logging

Worker akan menampilkan log untuk:
- Status sinkronisasi
- Node yang dicek
- Bot yang ditambah/dihapus
- Error yang terjadi

## Graceful Shutdown

Worker mendukung graceful shutdown dengan:
- `Ctrl+C` (SIGINT)
- `SIGTERM`

Database connection akan ditutup dengan benar saat shutdown.

## Troubleshooting

### Node Tidak Terjangkau
- Pastikan node berjalan dan dapat diakses
- Cek firewall dan network connectivity
- Periksa konfigurasi host dan port di database

### Database Error
- Pastikan DATABASE_URL benar
- Jalankan `npx prisma generate` jika ada perubahan schema
- Cek permission file database

### Bot Tidak Tersinkronisasi
- Periksa response API `/api/bots` dari node
- Pastikan format response sesuai dengan yang diharapkan
- Cek log untuk error detail