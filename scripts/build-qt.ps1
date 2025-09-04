# Qt Application Build Script
# Supports automatic Qt path detection, Visual Studio environment initialization and multiple build configurations

param(
    [string]$QtPath = "",
    [ValidateSet("Debug", "Release")]
    [string]$BuildType = "Release",
    [ValidateSet("Visual Studio 17 2022", "Visual Studio 16 2019", "Ninja")]
    [string]$Generator = "Visual Studio 17 2022",
    [ValidateSet("x64", "Win32")]
    [string]$Architecture = "x64",
    [switch]$Clean,
    [switch]$Help
)

# Show help information
if ($Help) {
    Write-Host "Qt Application Build Script" -ForegroundColor Cyan
    Write-Host "Usage: .\build-qt.ps1 [parameters]" -ForegroundColor White
    Write-Host ""
    Write-Host "Parameters:" -ForegroundColor Yellow
    Write-Host "  -QtPath <path>        Specify Qt installation path (optional, auto-detected)" -ForegroundColor White
    Write-Host "  -BuildType <type>     Build type: Debug or Release (default: Release)" -ForegroundColor White
    Write-Host "  -Generator <gen>      CMake generator (default: Visual Studio 17 2022)" -ForegroundColor White
    Write-Host "  -Architecture <arch>  Target architecture: x64 or Win32 (default: x64)" -ForegroundColor White
    Write-Host "  -Clean               Clean build directory" -ForegroundColor White
    Write-Host "  -Help                Show this help information" -ForegroundColor White
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Yellow
    Write-Host "  .\build-qt.ps1" -ForegroundColor Green
    Write-Host "  .\build-qt.ps1 -BuildType Debug" -ForegroundColor Green
    Write-Host "  .\build-qt.ps1 -QtPath 'C:\Qt\6.5.0\msvc2022_64'" -ForegroundColor Green
    Write-Host "  .\build-qt.ps1 -Clean" -ForegroundColor Green
    exit 0
}

# Function to check if command exists
function Test-Command {
    param([string]$Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    }
    catch {
        return $false
    }
}

Write-Host "=== Qt Application Build Script ===" -ForegroundColor Cyan
Write-Host "Build Type: $BuildType" -ForegroundColor White
Write-Host "Generator: $Generator" -ForegroundColor White
Write-Host "Architecture: $Architecture" -ForegroundColor White
Write-Host ""

# Check CMake
Write-Host "Checking CMake..." -ForegroundColor Yellow
if (-not (Test-Command "cmake")) {
    Write-Error "CMake not found. Please install CMake and add it to PATH environment variable"
}
$cmakeVersion = cmake --version | Select-Object -First 1
Write-Host "✓ Found CMake: $cmakeVersion" -ForegroundColor Green

# Initialize Visual Studio environment function
function Initialize-VSEnvironment {
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vsWhere)) {
        return $false
    }
    
    $vsPath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $vsPath) {
        return $false
    }
    
    $vcvarsPath = Join-Path $vsPath "VC\Auxiliary\Build\vcvars64.bat"
    if (-not (Test-Path $vcvarsPath)) {
        return $false
    }
    
    Write-Host "Found Visual Studio: $vsPath" -ForegroundColor Green
    Write-Host "Initializing Visual Studio environment..." -ForegroundColor Yellow
    
    # Run vcvars64.bat and get environment variables
    $tempFile = [System.IO.Path]::GetTempFileName()
    $batchContent = "@echo off`ncall `"$vcvarsPath`"`nset"
    
    Set-Content -Path $tempFile -Value $batchContent
    
    try {
        $envOutput = & cmd.exe /c $tempFile
        
        # Parse and set environment variables
        foreach ($line in $envOutput) {
            if ($line -match '^([^=]+)=(.*)$') {
                $name = $matches[1]
                $value = $matches[2]
                
                # Only set important build-related environment variables
                if ($name -match '^(PATH|INCLUDE|LIB|LIBPATH|VCINSTALLDIR|VSINSTALLDIR|WindowsSdkDir)$') {
                    [Environment]::SetEnvironmentVariable($name, $value, 'Process')
                }
            }
        }
        
        Write-Host "✓ Visual Studio environment initialized" -ForegroundColor Green
        return $true
    }
    catch {
        Write-Warning "Error initializing Visual Studio environment: $_"
        return $false
    }
    finally {
        if (Test-Path $tempFile) {
            Remove-Item $tempFile -Force
        }
    }
}

# Check Visual Studio build tools
Write-Host "Checking Visual Studio build tools..." -ForegroundColor Yellow
if (-not (Test-Command "cl")) {
    Write-Host "cl.exe not found in PATH, trying to initialize Visual Studio environment..." -ForegroundColor Yellow
    
    if (-not (Initialize-VSEnvironment)) {
        Write-Error "Visual Studio build tools not found or cannot be initialized. Please install Visual Studio 2022 or Build Tools"
    }
    
    # Check cl command again
    if (-not (Test-Command "cl")) {
        Write-Error "cl.exe still not found after Visual Studio environment initialization. Please check Visual Studio installation"
    }
}
Write-Host "✓ Found Visual Studio build tools" -ForegroundColor Green

# Auto-detect Qt path function
function Find-QtPath {
    $commonPaths = @(
        "C:\Qt\6.*\msvc2022_64",
        "C:\Qt\6.*\msvc2019_64",
        "${env:ProgramFiles}\Qt\6.*\msvc2022_64",
        "${env:ProgramFiles(x86)}\Qt\6.*\msvc2022_64"
    )
    
    foreach ($pattern in $commonPaths) {
        $basePath = Split-Path $pattern
        if (Test-Path $basePath) {
            $paths = Get-ChildItem -Path $basePath -Directory -ErrorAction SilentlyContinue | 
                     Where-Object { $_.Name -match "^6\." } |
                     Sort-Object Name -Descending
            
            foreach ($path in $paths) {
                $qtPath = Join-Path $path.FullName "msvc2022_64"
                if (-not (Test-Path $qtPath)) {
                    $qtPath = Join-Path $path.FullName "msvc2019_64"
                }
                
                if (Test-Path $qtPath) {
                    $qmakePath = Join-Path $qtPath "bin\qmake.exe"
                    if (Test-Path $qmakePath) {
                        return $qtPath
                    }
                }
            }
        }
    }
    
    return $null
}

# Check Qt
Write-Host "Checking Qt..." -ForegroundColor Yellow
if (-not $QtPath) {
    Write-Host "Qt path not specified, trying auto-detection..." -ForegroundColor Yellow
    $QtPath = Find-QtPath
    if (-not $QtPath) {
        Write-Error "Qt installation not found. Please install Qt6 or use -QtPath parameter to specify path"
    }
    Write-Host "Auto-detected Qt path: $QtPath" -ForegroundColor Green
} else {
    if (-not (Test-Path $QtPath)) {
        Write-Error "Specified Qt path does not exist: $QtPath"
    }
}

$qmakePath = Join-Path $QtPath "bin\qmake.exe"
if (-not (Test-Path $qmakePath)) {
    Write-Error "qmake.exe not found in Qt path: $QtPath"
}

$qtVersion = & $qmakePath -query QT_VERSION
Write-Host "✓ Found Qt: $qtVersion (Path: $QtPath)" -ForegroundColor Green

# Set build directory
$buildDir = "build"
if ($Clean -and (Test-Path $buildDir)) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item $buildDir -Recurse -Force
    Write-Host "✓ Build directory cleaned" -ForegroundColor Green
}

if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# Set CMake arguments
$cmakeArgs = @(
    "-S", ".",
    "-B", $buildDir,
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DCMAKE_PREFIX_PATH=$QtPath"
)

if ($Generator) {
    $cmakeArgs += "-G", $Generator
    if ($Generator -like "*Visual Studio*" -and $Architecture) {
        $cmakeArgs += "-A", $Architecture
    }
}

# Run CMake configuration
Write-Host "Running CMake configuration..." -ForegroundColor Yellow
Write-Host "Command: cmake $($cmakeArgs -join ' ')" -ForegroundColor Gray

try {
    & cmake @cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }
    Write-Host "✓ CMake configuration successful" -ForegroundColor Green
}
catch {
    Write-Error "CMake configuration failed: $_"
}

# Run build
Write-Host "Starting build..." -ForegroundColor Yellow
try {
    & cmake --build $buildDir --config $BuildType
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }
    Write-Host "✓ Build successful" -ForegroundColor Green
}
catch {
    Write-Error "Build failed: $_"
}

# Find generated executable files
$exePattern = "$buildDir\**\*.exe"
$exeFiles = Get-ChildItem -Path $exePattern -Recurse -ErrorAction SilentlyContinue

if ($exeFiles) {
    Write-Host "" 
    Write-Host "Build completed! Generated executable files:" -ForegroundColor Green
    foreach ($exe in $exeFiles) {
        Write-Host "  $($exe.FullName)" -ForegroundColor White
    }
    
    Write-Host ""
    Write-Host "Note: Before running executable files, ensure Qt DLL files are in PATH or in the same directory as the executable" -ForegroundColor Yellow
    Write-Host "You can use .\run.bat or .\run-qt.ps1 scripts to launch the application" -ForegroundColor Yellow
} else {
    Write-Warning "No generated executable files found"
}

Write-Host ""
Write-Host "=== Build Complete ===" -ForegroundColor Cyan