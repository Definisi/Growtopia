@echo off
REM Growtopia Bot API Test Script
REM Script untuk testing API endpoints menggunakan curl

echo ========================================
echo Growtopia Bot API Test Script
echo ========================================
echo.

set API_URL=http://localhost:8080/api
set BOT_NAME=testbot

echo Testing API endpoints...
echo.

REM Test 1: List all clients
echo [1] Testing list_clients endpoint...
curl -X POST %API_URL% ^
  -H "Content-Type: application/json" ^
  -d "{\"option\": \"list_clients\"}"
echo.
echo.

REM Test 2: Get client info
echo [2] Testing client info endpoint...
curl -X POST %API_URL% ^
  -H "Content-Type: application/json" ^
  -d "{\"option\": \"client\", \"tank_id_name\": \"%BOT_NAME%\"}"
echo.
echo.

REM Test 3: Get inventory
echo [3] Testing inventory endpoint...
curl -X POST %API_URL% ^
  -H "Content-Type: application/json" ^
  -d "{\"option\": \"inventory\", \"tank_id_name\": \"%BOT_NAME%\"}"
echo.
echo.

REM Test 4: Get player info
echo [4] Testing player_info endpoint...
curl -X POST %API_URL% ^
  -H "Content-Type: application/json" ^
  -d "{\"option\": \"player_info\", \"tank_id_name\": \"%BOT_NAME%\"}"
echo.
echo.

REM Test 5: Get world info
echo [5] Testing world_info endpoint...
curl -X POST %API_URL% ^
  -H "Content-Type: application/json" ^
  -d "{\"option\": \"world_info\", \"tank_id_name\": \"%BOT_NAME%\"}"
echo.
echo.

REM Test 6: Get status
echo [6] Testing status endpoint...
curl -X POST %API_URL% ^
  -H "Content-Type: application/json" ^
  -d "{\"option\": \"status\", \"tank_id_name\": \"%BOT_NAME%\"}"
echo.
echo.

REM Test 7: Get ping
echo [7] Testing ping endpoint...
curl -X POST %API_URL% ^
  -H "Content-Type: application/json" ^
  -d "{\"option\": \"ping\", \"tank_id_name\": \"%BOT_NAME%\"}"
echo.
echo.

REM Test 8: Move bot
echo [8] Testing move endpoint...
curl -X POST %API_URL% ^
  -H "Content-Type: application/json" ^
  -d "{\"option\": \"move\", \"tank_id_name\": \"%BOT_NAME%\", \"x\": 100, \"y\": 200}"
echo.
echo.

REM Test 9: Collect items
echo [9] Testing collect endpoint...
curl -X POST %API_URL% ^
  -H "Content-Type: application/json" ^
  -d "{\"option\": \"collect\", \"tank_id_name\": \"%BOT_NAME%\", \"range\": 3, \"force\": false}"
echo.
echo.

REM Test 10: Health check
echo [10] Testing health check endpoint...
curl -X GET %API_URL%/ok
echo.
echo.

echo ========================================
echo API Testing completed!
echo ========================================
echo.
echo Note: Make sure the Growtopia Bot application is running
echo and you have a bot named '%BOT_NAME%' added to test properly.
echo.
echo To add a bot, use:
echo curl -X POST %API_URL% ^
echo   -H "Content-Type: application/json" ^
echo   -d "{\"option\": \"add\", \"tank_id_name\": \"%BOT_NAME%\", \"tank_id_pass\": \"your_password\", \"server\": \"growtopia1.com\", \"port\": 17091}"
echo.
pause