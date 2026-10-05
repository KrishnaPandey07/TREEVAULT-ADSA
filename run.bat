@echo off
cls
echo ================================================================
echo           TreeVault — Multi-Language Backend Launcher
echo ================================================================
echo.
echo Select which backend engine to run the website on:
echo.
echo   [1] C++ Backend Server (Native Compiled Binary, Port 8000)
echo   [2] Java Backend Server (Native Java HTTP Server, Port 8080)
echo   [3] Python Backend Server (FastAPI + SQLite, Port 8000)
echo   [4] C++ Console Driver (Terminal Menu + Automated Tests)
echo   [5] Java Console Driver (Terminal Menu + Automated Tests)
echo   [0] Exit
echo.
echo ================================================================
set /p choice="Enter your choice [0-5]: "

if "%choice%"=="1" (
    echo.
    echo Starting C++ Native Web Server...
    cd /d "%~dp0backend_cpp"
    start "" "..\backend_cpp\TreeVault_Server.exe" 8000
    timeout /t 2 >nul
    start http://localhost:8000
    exit /b
)

if "%choice%"=="2" (
    echo.
    echo Starting Java Native Web Server...
    cd /d "%~dp0backend_java"
    call build.bat
    exit /b
)

if "%choice%"=="3" (
    echo.
    echo Starting Python FastAPI Web Server...
    cd /d "%~dp0backend"
    call .\venv\Scripts\uvicorn.exe main:app --host 127.0.0.1 --port 8000
    exit /b
)

if "%choice%"=="4" (
    echo.
    echo Launching C++ Console Application...
    cd /d "%~dp0backend_cpp"
    .\avl_tree.exe
    pause
    exit /b
)

if "%choice%"=="5" (
    echo.
    echo Launching Java Console Application...
    cd /d "%~dp0backend_java"
    if not exist "bin" mkdir bin
    javac -d bin src\com\treevault\*.java
    java -cp bin com.treevault.Main
    pause
    exit /b
)

exit /b
