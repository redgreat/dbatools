@echo off
chcp 65001 > nul
echo ========================================
echo    DBA Tools API 启动脚本
echo ========================================
echo.

:: 检查是否在正确的目录
if not exist "api\main.py" (
    echo 错误：请在项目根目录运行此脚本
    pause
    exit /b 1
)

:: 进入API目录
cd api

:: 检查Python是否安装
python --version >nul 2>&1
if errorlevel 1 (
    echo 错误：未找到Python，请先安装Python 3.8+
    pause
    exit /b 1
)

:: 检查虚拟环境
if not exist "venv" (
    echo 创建虚拟环境...
    python -m venv venv
    if errorlevel 1 (
        echo 错误：创建虚拟环境失败
        pause
        exit /b 1
    )
)

:: 激活虚拟环境
echo 激活虚拟环境...
call venv\Scripts\activate.bat
if errorlevel 1 (
    echo 错误：激活虚拟环境失败
    pause
    exit /b 1
)

:: 安装依赖
echo 检查并安装依赖...
pip install -r requirements.txt
if errorlevel 1 (
    echo 错误：安装依赖失败
    pause
    exit /b 1
)

:: 检查环境配置文件
if not exist ".env" (
    echo 创建环境配置文件...
    copy ".env.example" ".env"
    echo.
    echo 警告：请编辑 .env 文件配置数据库连接等信息
    echo.
)

:: 启动服务
echo 启动 FastAPI 服务...
echo 服务地址: http://localhost:8000
echo API 文档: http://localhost:8000/docs
echo 按 Ctrl+C 停止服务
echo.
uvicorn main:app --host 0.0.0.0 --port 8000 --reload

echo.
echo 服务已停止
pause