@echo off
chcp 65001 > nul
echo ========================================
echo    DBA Tools API 测试脚本
echo ========================================
echo.

set API_BASE=http://localhost:8000
set TEST_USERNAME=testuser
set TEST_EMAIL=test@example.com
set TEST_PASSWORD=testpass123

:: 检查curl是否可用
curl --version >nul 2>&1
if errorlevel 1 (
    echo 错误：未找到curl命令，请安装curl或使用Git Bash
    pause
    exit /b 1
)

echo 测试API服务连接性...
echo.

:: 1. 健康检查
echo [1/6] 健康检查...
curl -s %API_BASE%/health
if errorlevel 1 (
    echo 错误：API服务未启动或无法连接
    echo 请先启动API服务: scripts\start-api.bat
    pause
    exit /b 1
)
echo ✓ 健康检查通过
echo.

:: 2. 获取API文档
echo [2/6] 检查API文档...
curl -s -o nul -w "%%{http_code}" %API_BASE%/docs | findstr "200" >nul
if errorlevel 1 (
    echo ✗ API文档访问失败
) else (
    echo ✓ API文档可访问: %API_BASE%/docs
)
echo.

:: 3. 用户注册
echo [3/6] 测试用户注册...
set REGISTER_DATA={"username":"%TEST_USERNAME%","email":"%TEST_EMAIL%","password":"%TEST_PASSWORD%","full_name":"Test User"}
curl -s -X POST "%API_BASE%/auth/register" ^
     -H "Content-Type: application/json" ^
     -d "%REGISTER_DATA%" ^
     -w "\nHTTP Status: %%{http_code}\n"
echo.

:: 4. 用户登录
echo [4/6] 测试用户登录...
set LOGIN_DATA={"username":"%TEST_USERNAME%","password":"%TEST_PASSWORD%"}
for /f "tokens=*" %%i in ('curl -s -X POST "%API_BASE%/auth/login" -H "Content-Type: application/json" -d "%LOGIN_DATA%"') do set LOGIN_RESPONSE=%%i
echo %LOGIN_RESPONSE%
echo.

:: 提取访问令牌 (简化版本，实际使用中可能需要JSON解析工具)
for /f "tokens=2 delims=:,\"" %%a in ("%LOGIN_RESPONSE%") do (
    if "%%a"=="access_token" (
        for /f "tokens=4 delims=:,\"" %%b in ("%LOGIN_RESPONSE%") do set ACCESS_TOKEN=%%b
    )
)

if defined ACCESS_TOKEN (
    echo ✓ 登录成功，获取到访问令牌
) else (
    echo ✗ 登录失败或无法获取访问令牌
    set ACCESS_TOKEN=dummy_token
)
echo.

:: 5. 获取当前用户信息
echo [5/6] 测试获取用户信息...
curl -s -X GET "%API_BASE%/users/me" ^
     -H "Authorization: Bearer %ACCESS_TOKEN%" ^
     -w "\nHTTP Status: %%{http_code}\n"
echo.

:: 6. 获取角色列表
echo [6/6] 测试获取角色列表...
curl -s -X GET "%API_BASE%/roles/" ^
     -H "Authorization: Bearer %ACCESS_TOKEN%" ^
     -w "\nHTTP Status: %%{http_code}\n"
echo.

echo ========================================
echo 测试完成！
echo.
echo 如果所有测试都通过，说明API服务运行正常。
echo 你可以访问 %API_BASE%/docs 查看完整的API文档。
echo ========================================
echo.
pause