@echo off
REM Visual Studio Build Environment Setup Script (Batch Version)
REM Calls PowerShell script for environment detection and configuration

setlocal enabledelayedexpansion

echo ========================================
echo Visual Studio Build Environment Setup
echo ========================================
echo.

REM Check if PowerShell is available
powershell -Command "Write-Host 'PowerShell Available' -ForegroundColor Green" >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: PowerShell is not available
    echo Please ensure PowerShell is installed on your system
    pause
    exit /b 1
)

REM Get script directory
set "SCRIPT_DIR=%~dp0"
set "PS_SCRIPT=%SCRIPT_DIR%setup-vs-env.ps1"

REM Check if PowerShell script exists
if not exist "%PS_SCRIPT%" (
    echo Error: PowerShell script not found
    echo Path: %PS_SCRIPT%
    pause
    exit /b 1
)

echo Running Visual Studio environment detection...
echo.

REM Run PowerShell script
powershell -ExecutionPolicy Bypass -File "%PS_SCRIPT%" %*

if %errorlevel% neq 0 (
    echo.
    echo Error occurred during configuration
    pause
    exit /b %errorlevel%
)

echo.
echo Configuration completed!
pause