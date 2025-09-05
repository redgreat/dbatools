# 构建脚本 - 使用MinGW编译器
# 构建dbatools项目

Write-Host "正在构建dbatools项目 (MinGW)..." -ForegroundColor Green

# 检查build目录是否存在
if (-not (Test-Path "build")) {
    Write-Host "build目录不存在，请先运行configure-mingw.ps1" -ForegroundColor Red
    exit 1
}

# 设置环境变量绕过Qt许可证检查
$env:QTFRAMEWORK_BYPASS_LICENSE_CHECK = "1"

# 构建项目
Write-Host "开始构建..." -ForegroundColor Cyan
cmake --build "build" --config Debug

if ($LASTEXITCODE -eq 0) {
    Write-Host "构建成功完成！" -ForegroundColor Green
    Write-Host "可执行文件位置: build\dbatools.exe" -ForegroundColor Yellow
} else {
    Write-Host "构建失败，退出码: $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}