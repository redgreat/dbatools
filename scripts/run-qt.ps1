# DBA Tools Qt应用启动脚本
# PowerShell版本
# 作者: DBA Tools Team
# 版本: 1.0.0

param(
    [string]$BuildType = "Release",
    [string]$QtPath = "",
    [switch]$Verbose,
    [switch]$Help
)

# 显示帮助信息
function Show-Help {
    Write-Host "DBA Tools Qt应用启动脚本" -ForegroundColor Green
    Write-Host "用法: .\run-qt.ps1 [参数]" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "参数:" -ForegroundColor Cyan
    Write-Host "  -BuildType <类型>     构建类型 (Debug/Release，默认: Release)"
    Write-Host "  -QtPath <路径>        Qt安装路径 (可选，自动检测)"
    Write-Host "  -Verbose             详细输出"
    Write-Host "  -Help                显示此帮助信息"
    Write-Host ""
    Write-Host "示例:" -ForegroundColor Cyan
    Write-Host "  .\run-qt.ps1"
    Write-Host "  .\run-qt.ps1 -BuildType Debug -Verbose"
}

# 检查是否需要显示帮助
if ($Help) {
    Show-Help
    exit 0
}

# 设置错误处理
$ErrorActionPreference = "Stop"

# 获取脚本目录和项目根目录
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir
$BuildDir = Join-Path $ProjectRoot "build"

Write-Host "========================================" -ForegroundColor Green
Write-Host "DBA Tools Qt应用启动脚本" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "项目根目录: $ProjectRoot" -ForegroundColor Cyan
Write-Host "构建目录: $BuildDir" -ForegroundColor Cyan
Write-Host "构建类型: $BuildType" -ForegroundColor Cyan
Write-Host ""

# 自动检测Qt路径函数
function Find-QtPath {
    $commonPaths = @(
        "C:\Qt\6.*\msvc2022_64",
        "C:\Qt\6.*\msvc2019_64",
        "${env:ProgramFiles}\Qt\6.*\msvc2022_64",
        "${env:ProgramFiles(x86)}\Qt\6.*\msvc2022_64"
    )
    
    foreach ($pattern in $commonPaths) {
        $paths = Get-ChildItem -Path (Split-Path $pattern) -Directory -ErrorAction SilentlyContinue | 
                 Where-Object { $_.Name -match (Split-Path $pattern -Leaf).Replace('*', '.*') } |
                 Sort-Object Name -Descending
        
        foreach ($path in $paths) {
            $qmakePath = Join-Path $path.FullName "bin\qmake.exe"
            if (Test-Path $qmakePath) {
                return $path.FullName
            }
        }
    }
    
    return $null
}

# 检查构建目录是否存在
if (-not (Test-Path $BuildDir)) {
    Write-Error "构建目录不存在: $BuildDir`n请先运行构建脚本: .\build-qt.ps1"
}

# 查找可执行文件
$exePaths = @(
    "$BuildDir\$BuildType\dbatools.exe",
    "$BuildDir\dbatools.exe"
)

$exePath = $null
foreach ($path in $exePaths) {
    if (Test-Path $path) {
        $exePath = $path
        break
    }
}

if (-not $exePath) {
    Write-Error "未找到可执行文件，请检查构建是否成功`n查找路径: $($exePaths -join ', ')"
}

Write-Host "找到可执行文件: $exePath" -ForegroundColor Green

# 检查Qt路径
if ([string]::IsNullOrEmpty($QtPath)) {
    $QtPath = Find-QtPath
    if ([string]::IsNullOrEmpty($QtPath)) {
        Write-Warning "未找到Qt安装路径，可能需要手动设置PATH环境变量"
    } else {
        Write-Host "检测到Qt路径: $QtPath" -ForegroundColor Green
    }
}

# 设置Qt DLL路径
if (-not [string]::IsNullOrEmpty($QtPath)) {
    $qtBinPath = Join-Path $QtPath "bin"
    if (Test-Path $qtBinPath) {
        $env:PATH = "$qtBinPath;$env:PATH"
        Write-Host "已添加Qt bin目录到PATH: $qtBinPath" -ForegroundColor Green
    }
}

# 检查依赖的DLL文件
Write-Host "检查Qt依赖..." -ForegroundColor Yellow
$requiredDlls = @(
    "Qt6Core.dll",
    "Qt6Gui.dll",
    "Qt6Widgets.dll",
    "Qt6Network.dll"
)

$missingDlls = @()
foreach ($dll in $requiredDlls) {
    $found = $false
    
    # 检查可执行文件目录
    $exeDir = Split-Path $exePath
    if (Test-Path (Join-Path $exeDir $dll)) {
        $found = $true
    }
    
    # 检查PATH中的目录
    if (-not $found) {
        foreach ($pathDir in ($env:PATH -split ';')) {
            if ($pathDir -and (Test-Path (Join-Path $pathDir $dll))) {
                $found = $true
                break
            }
        }
    }
    
    if ($found) {
        if ($Verbose) {
            Write-Host "  ✓ $dll" -ForegroundColor Green
        }
    } else {
        $missingDlls += $dll
    }
}

if ($missingDlls.Count -gt 0) {
    Write-Warning "缺少以下Qt DLL文件: $($missingDlls -join ', ')"
    Write-Host "建议解决方案:" -ForegroundColor Yellow
    Write-Host "1. 确保Qt bin目录在PATH环境变量中" -ForegroundColor Yellow
    Write-Host "2. 或者将Qt DLL文件复制到可执行文件目录" -ForegroundColor Yellow
    Write-Host "3. 或者使用windeployqt工具部署应用" -ForegroundColor Yellow
    Write-Host ""
}

# 启动应用程序
Write-Host "========================================" -ForegroundColor Green
Write-Host "启动DBA Tools应用程序..." -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Green
Write-Host "可执行文件: $exePath" -ForegroundColor Cyan
Write-Host ""

try {
    # 获取文件信息
    $fileInfo = Get-Item $exePath
    Write-Host "文件大小: $([math]::Round($fileInfo.Length / 1MB, 2)) MB" -ForegroundColor Gray
    Write-Host "修改时间: $($fileInfo.LastWriteTime)" -ForegroundColor Gray
    Write-Host ""
    
    # 启动应用程序
    Write-Host "正在启动应用程序..." -ForegroundColor Green
    Start-Process -FilePath $exePath -WorkingDirectory (Split-Path $exePath)
    
    Write-Host "✓ 应用程序已启动" -ForegroundColor Green
    Write-Host "如果应用程序无法启动，请检查上述依赖项" -ForegroundColor Yellow
}
catch {
    Write-Error "启动应用程序失败: $_"
}