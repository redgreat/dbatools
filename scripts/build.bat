@echo off
REM DBA Tools 构建脚本
REM 适用于Windows系统

echo ========================================
echo DBA Tools 构建脚本
echo ========================================

REM 检查是否存在build目录
if exist "build" (
    echo 清理旧的构建目录...
    rmdir /s /q build
)

REM 创建构建目录
echo 创建构建目录...
mkdir build
cd build

REM 运行CMake配置
echo 配置项目...
cmake .. -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 (
    echo CMake配置失败！
    pause
    exit /b 1
)

REM 构建项目
echo 构建项目...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo 构建失败！
    pause
    exit /b 1
)

echo ========================================
echo 构建完成！
echo 可执行文件位置: build\Release\dbatools.exe
echo ========================================

pause