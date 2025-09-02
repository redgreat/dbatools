@echo off
chcp 65001 > nul
echo ========================================
echo    DBA Tools API Docker 启动脚本
echo ========================================
echo.

:: 检查是否在正确的目录
if not exist "api\docker-compose.yml" (
    echo 错误：请在项目根目录运行此脚本
    pause
    exit /b 1
)

:: 检查Docker是否安装
docker --version >nul 2>&1
if errorlevel 1 (
    echo 错误：未找到Docker，请先安装Docker Desktop
    pause
    exit /b 1
)

:: 检查Docker Compose是否安装
docker compose version >nul 2>&1
if errorlevel 1 (
    echo 错误：未找到Docker Compose，请确保Docker Desktop已正确安装
    pause
    exit /b 1
)

:: 进入API目录
cd api

echo 选择启动模式：
echo 1. 开发模式 (包含数据库)
echo 2. 生产模式 (包含Nginx)
echo 3. 仅API服务
echo 4. 停止所有服务
echo 5. 查看服务状态
echo 6. 查看日志
set /p choice=请选择 (1-6): 

if "%choice%"=="1" (
    echo 启动开发环境...
    docker compose up -d postgres redis
    timeout /t 10 /nobreak >nul
    docker compose up api
) else if "%choice%"=="2" (
    echo 启动生产环境...
    docker compose --profile production up -d
) else if "%choice%"=="3" (
    echo 启动API服务...
    docker compose up api
) else if "%choice%"=="4" (
    echo 停止所有服务...
    docker compose down
    echo 服务已停止
) else if "%choice%"=="5" (
    echo 查看服务状态...
    docker compose ps
) else if "%choice%"=="6" (
    echo 查看日志...
    docker compose logs -f
) else (
    echo 无效选择
)

echo.
echo 常用命令：
echo - 查看API文档: http://localhost:8000/docs
echo - 查看服务状态: docker compose ps
echo - 查看日志: docker compose logs -f [service_name]
echo - 停止服务: docker compose down
echo - 重建镜像: docker compose build --no-cache
echo.
pause