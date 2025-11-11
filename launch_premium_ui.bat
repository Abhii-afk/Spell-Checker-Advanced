@echo off
echo ========================================
echo   Premium Spell Checker UI Launcher
echo   Team Ctrl Alt Defeat
echo ========================================
echo.

REM Check if server is built
if not exist spell_checker_web.exe (
    echo Building web server...
    call build_web_server.bat
    if errorlevel 1 (
        echo Failed to build server!
        pause
        exit /b 1
    )
    echo.
)

REM Start the backend server
echo Starting backend server on port 8080...
start "Spell Checker API Server" cmd /k "spell_checker_web.exe"

REM Wait for server to start
echo Waiting for server to initialize...
timeout /t 3 /nobreak > nul

REM Open the premium UI in default browser
echo Opening Premium UI in browser...
start "" "web\premium_index.html"

echo.
echo ========================================
echo   Premium UI is now running!
echo ========================================
echo.
echo Backend API: http://localhost:8080
echo Frontend UI: Opened in your browser
echo.
echo Press any key to stop the server...
pause > nul

REM Stop the server
taskkill /f /im "spell_checker_web.exe" 2>nul
echo.
echo Server stopped. Thank you!
pause