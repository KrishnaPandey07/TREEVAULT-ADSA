@echo off
echo ===================================================
echo     Compiling TreeVault Java Backend (javac)
echo ===================================================
if not exist "bin" mkdir bin

javac -d bin src\com\treevault\*.java
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation failed! Ensure JDK is installed and on your PATH.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [SUCCESS] Java Compilation Succeeded!
echo.
echo Select Run Mode:
echo   [1] Interactive CLI Console (with full automated tests)
echo   [2] Standalone REST API Server (port 8080)
echo.
set /p choice="Enter choice [1 or 2]: "

if "%choice%"=="2" (
    echo Starting TreeVault Java REST Server on port 8080...
    java -cp bin com.treevault.TreeHttpServer
) else (
    echo Starting TreeVault Java Console...
    java -cp bin com.treevault.Main
)
pause
