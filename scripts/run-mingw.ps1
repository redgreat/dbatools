# 运行脚本 - dbatools (MinGW版本)
# 设置Qt运行环境并启动应用程序

Write-Host "正在启动dbatools应用程序..." -ForegroundColor Green

# 检查可执行文件是否存在
if (-not (Test-Path "build\dbatools.exe")) {
    Write-Host "可执行文件不存在，请先构建项目" -ForegroundColor Red
    Write-Host "运行: .\scripts\build-mingw.ps1" -ForegroundColor Yellow
    exit 1
}

# 设置Qt运行时库路径
$qtBinPath = "e:\Qt\6.9.2\llvm-mingw_64\bin"
if (-not (Test-Path $qtBinPath)) {
    Write-Host "Qt运行时库路径不存在: $qtBinPath" -ForegroundColor Red
    exit 1
}

# 添加Qt库到PATH环境变量
$env:PATH = "$qtBinPath;" + $env:PATH

# 设置Qt插件路径
$env:QT_PLUGIN_PATH = "e:\Qt\6.9.2\llvm-mingw_64\plugins"

# 启动应用程序
Write-Host "启动应用程序..." -ForegroundColor Cyan
Write-Host "Qt库路径: $qtBinPath" -ForegroundColor Gray
Write-Host "如需查看控制台输出，请保持此窗口打开" -ForegroundColor Yellow
Write-Host "按 Ctrl+C 可以终止应用程序" -ForegroundColor Yellow
Write-Host "" # 空行

# 运行应用程序并捕获输出
try {
    & ".\build\dbatools.exe"
    Write-Host "应用程序正常退出" -ForegroundColor Green
} catch {
    Write-Host "应用程序异常退出: $_" -ForegroundColor Red
    exit 1
}