# 调试运行脚本 - dbatools
# 启动应用程序并显示详细的调试信息

Write-Host "正在以调试模式启动dbatools..." -ForegroundColor Green

# 检查可执行文件
if (-not (Test-Path "build\dbatools.exe")) {
    Write-Host "可执行文件不存在，请先构建项目" -ForegroundColor Red
    exit 1
}

# 设置Qt环境
$qtBinPath = "e:\Qt\6.9.2\llvm-mingw_64\bin"
$env:PATH = "$qtBinPath;" + $env:PATH
$env:QT_PLUGIN_PATH = "e:\Qt\6.9.2\llvm-mingw_64\plugins"

# 启用Qt调试输出，过滤掉不必要的信息
$env:QT_LOGGING_RULES = "*.debug=false;*.info=false;*.warning=true;*.critical=true;qt.widgets.painting=false;qt.qpa.window=false;qt.accessibility.cache=false;qt.pointer.velocity=false;qt.text.drawing=false;qt.widgets.gestures=false;qt.qpa.events=false;dbatools.debug=true"
$env:QT_DEBUG_PLUGINS = "1"

# 显示环境信息
Write-Host "=== 调试信息 ===" -ForegroundColor Cyan
Write-Host "可执行文件: $(Resolve-Path 'build\dbatools.exe')" -ForegroundColor Gray
Write-Host "Qt库路径: $qtBinPath" -ForegroundColor Gray
Write-Host "Qt插件路径: $($env:QT_PLUGIN_PATH)" -ForegroundColor Gray
Write-Host "当前工作目录: $(Get-Location)" -ForegroundColor Gray
Write-Host "===============" -ForegroundColor Cyan
Write-Host ""

Write-Host "启动应用程序 (调试模式)..." -ForegroundColor Yellow
Write-Host "控制台输出将显示在下方:" -ForegroundColor Yellow
Write-Host "按 Ctrl+C 终止应用程序" -ForegroundColor Red
Write-Host "" # 空行

# 启动应用程序并实时显示输出
try {
    $process = Start-Process -FilePath ".\build\dbatools.exe" -NoNewWindow -PassThru -RedirectStandardOutput "debug_output.log" -RedirectStandardError "debug_error.log"
    
    Write-Host "应用程序已启动 (PID: $($process.Id))" -ForegroundColor Green
    Write-Host "监控日志文件..." -ForegroundColor Cyan
    
    # 监控日志文件
    $outputFile = "debug_output.log"
    $errorFile = "debug_error.log"
    
    while (-not $process.HasExited) {
        if (Test-Path $outputFile) {
            $content = Get-Content $outputFile -Tail 10 -ErrorAction SilentlyContinue
            if ($content) {
                Write-Host "[输出] $content" -ForegroundColor White
            }
        }
        
        if (Test-Path $errorFile) {
            $errorContent = Get-Content $errorFile -Tail 10 -ErrorAction SilentlyContinue
            if ($errorContent) {
                Write-Host "[错误] $errorContent" -ForegroundColor Red
            }
        }
        
        Start-Sleep -Milliseconds 500
    }
    
    Write-Host "应用程序已退出 (退出码: $($process.ExitCode))" -ForegroundColor $(if ($process.ExitCode -eq 0) { "Green" } else { "Red" })
    
    # 显示最终日志
    if (Test-Path $outputFile) {
        $finalOutput = Get-Content $outputFile -ErrorAction SilentlyContinue
        if ($finalOutput) {
            Write-Host "\n=== 完整输出日志 ===" -ForegroundColor Cyan
            $finalOutput | ForEach-Object { Write-Host $_ -ForegroundColor White }
        }
    }
    
    if (Test-Path $errorFile) {
        $finalError = Get-Content $errorFile -ErrorAction SilentlyContinue
        if ($finalError) {
            Write-Host "\n=== 错误日志 ===" -ForegroundColor Red
            $finalError | ForEach-Object { Write-Host $_ -ForegroundColor Red }
        }
    }
    
} catch {
    Write-Host "启动失败: $_" -ForegroundColor Red
    exit 1
} finally {
    # 清理临时日志文件
    Remove-Item "debug_output.log" -ErrorAction SilentlyContinue
    Remove-Item "debug_error.log" -ErrorAction SilentlyContinue
}