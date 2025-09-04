# Visual Studio Build Environment Setup Script
# PowerShell Version
# Author: DBA Tools Team
# Version: 1.0.0

param(
    [switch]$Help,
    [switch]$Verbose
)

# Show help information
function Show-Help {
    Write-Host "Visual Studio Build Environment Setup Script" -ForegroundColor Green
    Write-Host "Usage: .\setup-vs-env.ps1 [parameters]" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Parameters:" -ForegroundColor Cyan
    Write-Host "  -Verbose             Verbose output"
    Write-Host "  -Help                Show this help information"
    Write-Host ""
    Write-Host "Features:" -ForegroundColor Cyan
    Write-Host "  1. Detect Visual Studio installation"
    Write-Host "  2. Find build tool components"
    Write-Host "  3. Provide environment configuration suggestions"
    Write-Host "  4. Generate environment initialization script"
}

# Check if help is needed
if ($Help) {
    Show-Help
    exit 0
}

# Set error handling
$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Green
Write-Host "Visual Studio Build Environment Setup" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""

# Check vswhere tool
function Find-VSWhere {
    $vsWherePaths = @(
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe",
        "${env:ProgramFiles}\Microsoft Visual Studio\Installer\vswhere.exe"
    )
    
    foreach ($path in $vsWherePaths) {
        if (Test-Path $path) {
            return $path
        }
    }
    
    return $null
}

# Find Visual Studio installations
function Find-VisualStudio {
    $vsWhere = Find-VSWhere
    
    if (-not $vsWhere) {
        Write-Warning "vswhere.exe tool not found"
        return $null
    }
    
    Write-Host "Using vswhere to find Visual Studio installations..." -ForegroundColor Yellow
    
    try {
        # Find all Visual Studio installations
        $installations = & $vsWhere -format json | ConvertFrom-Json
        
        if ($installations) {
            Write-Host "Found the following Visual Studio installations:" -ForegroundColor Green
            
            foreach ($install in $installations) {
                Write-Host "  Product: $($install.displayName)" -ForegroundColor Cyan
                Write-Host "  Version: $($install.installationVersion)" -ForegroundColor Cyan
                Write-Host "  Path: $($install.installationPath)" -ForegroundColor Cyan
                Write-Host ""
            }
            
            return $installations
        }
    }
    catch {
        Write-Warning "Error running vswhere: $_"
    }
    
    return $null
}

# Check C++ build tools
function Test-CppBuildTools {
    param([string]$VSPath)
    
    $vcToolsPath = Join-Path $VSPath "VC\Tools\MSVC"
    $buildToolsPath = Join-Path $VSPath "MSBuild\Current\Bin"
    
    $results = @{
        VCTools = $false
        MSBuild = $false
        CMake = $false
        VCVars = $false
    }
    
    # Check VC++ tools
    if (Test-Path $vcToolsPath) {
        $vcVersions = Get-ChildItem $vcToolsPath -Directory | Sort-Object Name -Descending
        if ($vcVersions) {
            $results.VCTools = $true
            $latestVC = $vcVersions[0]
            $clPath = Join-Path $latestVC.FullName "bin\Hostx64\x64\cl.exe"
            if (Test-Path $clPath) {
                Write-Host "✓ Found VC++ compiler: $clPath" -ForegroundColor Green
            }
        }
    }
    
    # Check MSBuild
    if (Test-Path $buildToolsPath) {
        $msbuildPath = Join-Path $buildToolsPath "MSBuild.exe"
        if (Test-Path $msbuildPath) {
            $results.MSBuild = $true
            Write-Host "✓ Found MSBuild: $msbuildPath" -ForegroundColor Green
        }
    }
    
    # Check VCVars script
    $vcvarsPath = Join-Path $VSPath "VC\Auxiliary\Build\vcvars64.bat"
    if (Test-Path $vcvarsPath) {
        $results.VCVars = $true
        Write-Host "✓ Found VCVars script: $vcvarsPath" -ForegroundColor Green
    }
    
    # Check CMake
    $cmakePath = Join-Path $VSPath "Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    if (Test-Path $cmakePath) {
        $results.CMake = $true
        Write-Host "✓ Found Visual Studio CMake: $cmakePath" -ForegroundColor Green
    }
    
    return $results
}

# Generate environment initialization script
function New-VSEnvironmentScript {
    param([string]$VSPath)
    
    $scriptPath = Join-Path (Get-Location) "init-vs-env.bat"
    
    $vcvarsPath = Join-Path $VSPath "VC\Auxiliary\Build\vcvars64.bat"
    
    $content = @"
@echo off
REM Visual Studio Environment Initialization Script
REM Auto-generated on: $(Get-Date)

echo Initializing Visual Studio 2022 build environment...

REM Call vcvars64.bat to set environment variables
call "$vcvarsPath"

if %errorlevel% neq 0 (
    echo Error: Unable to initialize Visual Studio environment
    pause
    exit /b 1
)

echo ✓ Visual Studio environment initialized
echo.
echo You can now run the following commands:
echo   cmake --version
echo   cl
echo   msbuild
echo.
echo Or directly run build scripts:
echo   scripts\build-qt.ps1
echo.

REM Keep command prompt open
cmd /k
"@
    
    Set-Content -Path $scriptPath -Value $content -Encoding UTF8
    
    Write-Host "✓ Generated environment initialization script: $scriptPath" -ForegroundColor Green
    
    return $scriptPath
}

# Main detection logic
Write-Host "Detecting Visual Studio installation..." -ForegroundColor Yellow

$installations = Find-VisualStudio

if (-not $installations) {
    Write-Host "❌ No Visual Studio installation found" -ForegroundColor Red
    Write-Host ""
    Write-Host "Solutions:" -ForegroundColor Yellow
    Write-Host "1. Install Visual Studio 2022 Community (Free)" -ForegroundColor White
    Write-Host "   Download: https://visualstudio.microsoft.com/downloads/" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "2. Or install Visual Studio Build Tools 2022" -ForegroundColor White
    Write-Host "   Download: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "During installation, please ensure to select:" -ForegroundColor Yellow
    Write-Host "  - C++ CMake tools for Visual Studio" -ForegroundColor White
    Write-Host "  - MSVC v143 - VS 2022 C++ x64/x86 build tools" -ForegroundColor White
    Write-Host "  - Windows 10/11 SDK" -ForegroundColor White
    exit 1
}

# Check build tools for each installation
$validInstallations = @()

foreach ($install in $installations) {
    Write-Host "Checking installation: $($install.displayName)" -ForegroundColor Yellow
    
    $buildTools = Test-CppBuildTools -VSPath $install.installationPath
    
    if ($buildTools.VCTools -and $buildTools.VCVars) {
        $validInstallations += $install
        Write-Host "✓ This installation contains required C++ build tools" -ForegroundColor Green
    } else {
        Write-Host "❌ This installation is missing C++ build tools" -ForegroundColor Red
        
        if (-not $buildTools.VCTools) {
            Write-Host "  Missing: VC++ compiler" -ForegroundColor Red
        }
        if (-not $buildTools.VCVars) {
            Write-Host "  Missing: VCVars environment script" -ForegroundColor Red
        }
    }
    
    Write-Host ""
}

if ($validInstallations.Count -eq 0) {
    Write-Host "❌ All Visual Studio installations are missing required C++ build tools" -ForegroundColor Red
    Write-Host ""
    Write-Host "Solutions:" -ForegroundColor Yellow
    Write-Host "1. Open Visual Studio Installer" -ForegroundColor White
    Write-Host "2. Click 'Modify' button" -ForegroundColor White
    Write-Host "3. In 'Workloads' tab, select:" -ForegroundColor White
    Write-Host "   - Desktop development with C++" -ForegroundColor Cyan
    Write-Host "4. In 'Individual components' tab, ensure these are selected:" -ForegroundColor White
    Write-Host "   - MSVC v143 - VS 2022 C++ x64/x86 build tools" -ForegroundColor Cyan
    Write-Host "   - C++ CMake tools for Visual Studio" -ForegroundColor Cyan
    Write-Host "   - Windows 10/11 SDK (latest version)" -ForegroundColor Cyan
    Write-Host "5. Click 'Modify' to complete installation" -ForegroundColor White
    exit 1
}

# Use the first valid installation
$selectedInstall = $validInstallations[0]

Write-Host "========================================" -ForegroundColor Green
Write-Host "Build Environment Configuration" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "Selected Visual Studio: $($selectedInstall.displayName)" -ForegroundColor Cyan
Write-Host "Installation Path: $($selectedInstall.installationPath)" -ForegroundColor Cyan
Write-Host ""

# Generate environment initialization script
$envScript = New-VSEnvironmentScript -VSPath $selectedInstall.installationPath

Write-Host "========================================" -ForegroundColor Green
Write-Host "Usage Instructions" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Method 1: Use generated environment script (Recommended)" -ForegroundColor Yellow
Write-Host "  Double-click to run: $envScript" -ForegroundColor Cyan
Write-Host "  Then run build scripts in the opened command prompt" -ForegroundColor White
Write-Host ""
Write-Host "Method 2: Use Visual Studio Developer Command Prompt" -ForegroundColor Yellow
Write-Host "  1. Open Start Menu" -ForegroundColor White
Write-Host "  2. Search for 'Developer Command Prompt for VS 2022'" -ForegroundColor White
Write-Host "  3. Run build scripts in the opened command prompt" -ForegroundColor White
Write-Host ""
Write-Host "Method 3: Manually initialize environment in PowerShell" -ForegroundColor Yellow
$vcvarsPath = Join-Path $selectedInstall.installationPath "VC\Auxiliary\Build\vcvars64.bat"
Write-Host "  cmd /c '\"$vcvarsPath\" && powershell'" -ForegroundColor Cyan
Write-Host ""
Write-Host "Verify environment is correctly configured:" -ForegroundColor Yellow
Write-Host "  cl" -ForegroundColor Cyan
Write-Host "  cmake --version" -ForegroundColor Cyan
Write-Host ""
Write-Host "After configuration, run build script:" -ForegroundColor Yellow
Write-Host "  scripts\build-qt.ps1" -ForegroundColor Cyan
Write-Host ""