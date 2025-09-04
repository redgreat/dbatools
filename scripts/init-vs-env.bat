@echo off
REM Visual Studio Environment Initialization Script
REM Auto-generated on: 09/03/2025 09:59:08

echo Initializing Visual Studio 2022 build environment...

REM Call vcvars64.bat to set environment variables
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if %errorlevel% neq 0 (
    echo Error: Unable to initialize Visual Studio environment
    pause
    exit /b 1
)

echo 鉁?Visual Studio environment initialized
echo.
echo You can now run the following commands:
echo   cmake --version
echo   cl
echo   msbuild
echo.
echo Or directly run build scripts:
echo   scripts\build-qt.ps1
echo.

REM Keep command prompt open
cmd /k
