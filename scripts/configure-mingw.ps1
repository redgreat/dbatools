# CMake配置脚本 - 使用MinGW编译器
# 解决生成器冲突和Qt路径配置问题

Write-Host "正在配置dbatools项目 (MinGW)..." -ForegroundColor Green

# 清理build目录
if (Test-Path "build") {
    Write-Host "清理现有build目录..." -ForegroundColor Yellow
    Remove-Item -Path "build\*" -Recurse -Force -ErrorAction SilentlyContinue
}

# 创建build目录
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# 设置环境变量绕过Qt许可证检查
$env:QTFRAMEWORK_BYPASS_LICENSE_CHECK = "1"

# CMake配置参数
$cmakeArgs = @(
    "-DCMAKE_BUILD_TYPE=Debug",
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE",
    "-DCMAKE_C_COMPILER=e:\Qt\Tools\llvm-mingw1706_64\bin\clang.exe",
    "-DCMAKE_CXX_COMPILER=e:\Qt\Tools\llvm-mingw1706_64\bin\clang++.exe",
    "-DCMAKE_MAKE_PROGRAM=e:\Qt\Tools\llvm-mingw1706_64\bin\mingw32-make.exe",
    "-DCMAKE_PREFIX_PATH=e:\Qt\6.9.2\llvm-mingw_64",
    "-S", ".",
    "-B", "build",
    "-G", "MinGW Makefiles"
)

Write-Host "运行CMake配置..." -ForegroundColor Cyan
cmake @cmakeArgs

if ($LASTEXITCODE -eq 0) {
    Write-Host "配置成功完成！" -ForegroundColor Green
    Write-Host "使用以下命令构建项目:" -ForegroundColor Yellow
    Write-Host "  cmake --build build --config Debug" -ForegroundColor White
} else {
    Write-Host "配置失败，退出码: $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}