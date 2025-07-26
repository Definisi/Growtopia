@echo off
echo Starting Growtopia Bot Worker...
echo.

REM Check if node_modules exists
if not exist "node_modules" (
    echo Installing dependencies...
    npm install
    echo.
)

REM Generate Prisma client
echo Generating Prisma client...
npx prisma generate
echo.

REM Start the worker
echo Starting worker service...
node index.js

pause