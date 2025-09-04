@echo off
chcp 65001 >nul
REM DBA Tools Qt应用启动脚本 (批处理版本)
REM 调用PowerShell脚本启动应用程序

echo ========================================
echo DBA Tools Qt应用启动脚本
echo ========================================
echo.

REM 检查PowerShell是否可用
powershell -Command "Get-Host" >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 未找到PowerShell
    echo 请确保系统已安装PowerShell
    pause
    exit /b 1
)

REM 获取脚本目录
set "SCRIPT_DIR=%~dp0"
set "PS_SCRIPT=%SCRIPT_DIR%run-qt.ps1"

REM 检查PowerShell脚本是否存在
if not exist "%PS_SCRIPT%" (
    echo 错误: 未找到PowerShell启动脚本
    echo 路径: %PS_SCRIPT%
    pause
    exit /b 1
)

echo 调用PowerShell启动脚本...
echo.

REM 调用PowerShell脚本，传递所有参数
powershell -ExecutionPolicy Bypass -File "%PS_SCRIPT%" %*

REM 检查PowerShell脚本的退出代码
if %errorlevel% neq 0 (
    echo.
    echo 启动失败，退出代码: %errorlevel%
    pause
    exit /b %errorlevel%
)

echo.
echo 启动完成!
pause