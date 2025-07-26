@echo off
echo ========================================
echo   Stopping Growtopia Bot Worker
echo ========================================
echo.

REM Kill all node processes running worker
echo Stopping worker processes...
taskkill /f /im node.exe /fi "WINDOWTITLE eq Worker*" >nul 2>&1
taskkill /f /im node.exe /fi "IMAGENAME eq node.exe" >nul 2>&1

REM More specific kill for worker
for /f "tokens=2" %%i in ('tasklist /fi "imagename eq node.exe" /fo csv ^| find "node.exe"') do (
    taskkill /f /pid %%i >nul 2>&1
)

echo Worker stopped.
echo.
pause