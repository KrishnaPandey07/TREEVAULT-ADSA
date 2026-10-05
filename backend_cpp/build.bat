@echo off
echo ===================================================
echo     Compiling TreeVault C++ Backend (MinGW g++)
echo ===================================================
C:\MinGW\bin\g++.exe -std=c++11 -Wall -O2 main.cpp AVLTree.cpp -o avl_tree.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b %ERRORLEVEL%
)
echo.
echo [SUCCESS] Compilation complete: avl_tree.exe
echo Launching TreeVault C++ Interactive Console...
echo ===================================================
echo.
avl_tree.exe
pause
