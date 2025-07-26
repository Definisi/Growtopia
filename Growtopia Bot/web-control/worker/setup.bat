@echo off
echo ========================================
echo   Growtopia Bot Worker Setup
echo ========================================
echo.

REM Check if Node.js is installed
node --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Node.js is not installed or not in PATH
    echo Please install Node.js from https://nodejs.org/
    pause
    exit /b 1
)

echo Node.js version:
node --version
echo.

REM Install dependencies
echo Installing dependencies...
npm install
if %errorlevel% neq 0 (
    echo ERROR: Failed to install dependencies
    pause
    exit /b 1
)
echo.

REM Copy environment file if it doesn't exist
if not exist ".env" (
    echo Creating .env file...
    copy ".env.example" ".env" >nul 2>&1
    if not exist ".env" (
        echo DATABASE_URL="file:../prisma/dev.db" > .env
        echo SYNC_INTERVAL_MINUTES=5 >> .env
        echo NODE_TIMEOUT_MS=5000 >> .env
        echo BOT_API_TIMEOUT_MS=10000 >> .env
        echo LOG_LEVEL=info >> .env
        echo LOG_FILE=worker.log >> .env
    )
    echo .env file created
echo.
)

REM Generate Prisma client
echo Generating Prisma client...
npx prisma generate
if %errorlevel% neq 0 (
    echo ERROR: Failed to generate Prisma client
    pause
    exit /b 1
)
echo.

echo ========================================
echo   Setup completed successfully!
echo ========================================
echo.
echo To start the worker:
echo   npm start        (production)
echo   npm run dev      (development)
echo.
echo Configuration file: .env
echo Log file: worker.log
echo.
pause