@echo off
chcp 65001 >nul
REM DBA Tools Qt6 Installation Guide
REM For Windows Systems

echo ========================================
echo DBA Tools Qt6 Installation Guide
echo ========================================
echo.
echo This project requires Qt6 to compile the client program.
echo Please follow these steps to install Qt6:
echo.
echo Method 1: Qt Online Installer (Recommended)
echo 1. Visit https://www.qt.io/download-qt-installer
echo 2. Download Qt Online Installer for Windows
echo 3. Run installer and create Qt account (free)
echo 4. Select Qt 6.5 or higher version
echo 5. Make sure to select these components:
echo    - Qt 6.x.x
echo    - MSVC 2022 64-bit
echo    - Qt Creator (optional but recommended)
echo.
echo Method 2: Using vcpkg
echo If you have vcpkg installed, run:
echo vcpkg install qt6[core,widgets,network]:x64-windows
echo.
echo Method 3: Using Chocolatey
echo If you have Chocolatey installed, run:
echo choco install qt6
echo.
echo ========================================
echo Configuration after installation
echo ========================================
echo.
echo 1. Add Qt6 bin directory to system PATH
echo    Example: C:\Qt\6.5.0\msvc2022_64\bin
echo.
echo 2. Set CMAKE_PREFIX_PATH environment variable
echo    Example: C:\Qt\6.5.0\msvc2022_64
echo.
echo 3. Or specify Qt6 path in CMake command:
echo    cmake .. -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2022_64"
echo.
echo ========================================
echo Verify installation
echo ========================================
echo.
echo After installation, run this command to verify:
echo qmake --version
echo.
echo Then re-run the build script:
echo scripts\build.bat
echo.
echo ========================================

pause