# DBA Tools Build Test Script
Write-Host "DBA Tools Build Test" -ForegroundColor Green
Write-Host "===================" -ForegroundColor Green
Write-Host ""

# Check project files
Write-Host "1. Checking project files..." -ForegroundColor Yellow
$files = @(
    "CMakeLists.txt", "main.cpp", "mainwindow.cpp", "mainwindow.h",
    "loginwindow.cpp", "loginwindow.h", "apimanager.cpp", "apimanager.h",
    "stringformatter.cpp", "stringformatter.h", "usermanager.cpp", "usermanager.h",
    "usereditor.cpp", "usereditor.h", "rolemanager.cpp", "rolemanager.h",
    "roleeditor.cpp", "roleeditor.h"
)

$missing = @()
foreach ($file in $files) {
    if (Test-Path $file) {
        Write-Host "  Found: $file" -ForegroundColor Green
    } else {
        Write-Host "  Missing: $file" -ForegroundColor Red
        $missing += $file
    }
}

if ($missing.Count -eq 0) {
    Write-Host "All files present!" -ForegroundColor Green
} else {
    Write-Host "Missing $($missing.Count) files" -ForegroundColor Red
}

Write-Host ""

# Check Qt installation
Write-Host "2. Checking Qt installation..." -ForegroundColor Yellow
$qtFound = $false

try {
    $null = Get-Command qmake -ErrorAction Stop
    Write-Host "  qmake found" -ForegroundColor Green
    $qtFound = $true
} catch {
    Write-Host "  qmake not found" -ForegroundColor Red
}

$qtPaths = @("C:\Qt", "C:\Program Files\Qt")
foreach ($path in $qtPaths) {
    if (Test-Path $path) {
        Write-Host "  Qt directory found: $path" -ForegroundColor Green
        $qtFound = $true
        break
    }
}

if (-not $qtFound) {
    Write-Host "  Qt not installed" -ForegroundColor Red
}

Write-Host ""

# Summary
Write-Host "Summary:" -ForegroundColor Green
if ($missing.Count -eq 0) {
    Write-Host "Project files: Complete" -ForegroundColor Green
} else {
    Write-Host "Project files: Incomplete" -ForegroundColor Red
}

if ($qtFound) {
    Write-Host "Qt environment: Available" -ForegroundColor Green
} else {
    Write-Host "Qt environment: Not installed" -ForegroundColor Red
}

Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
if (-not $qtFound) {
    Write-Host "1. Install Qt (see scripts/install-qt.bat)" -ForegroundColor Cyan
} else {
    Write-Host "1. Run: cmake -B build -S ." -ForegroundColor Cyan
    Write-Host "2. Run: cmake --build build" -ForegroundColor Cyan
}