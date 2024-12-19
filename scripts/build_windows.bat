@echo off
setlocal enabledelayedexpansion

set VERSION=1.0.0
set PLUGIN_NAME=IKReverb

echo Building %PLUGIN_NAME% version %VERSION%...

:: Check if Visual Studio is installed
if not exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.com" (
    echo Error: Visual Studio 2022 Community not found.
    exit /b 1
)

:: Build VST3 plugin
echo Building VST3...
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.com" ^
    "Builds\VisualStudio2022\%PLUGIN_NAME%.sln" /Build "Release|x64"

if errorlevel 1 (
    echo Error: Build failed.
    exit /b 1
)

:: Create installer
echo Creating installer...
if not exist "scripts\NSIS" (
    echo Error: NSIS not found in scripts directory.
    exit /b 1
)

scripts\NSIS\makensis.exe scripts\windows_installer.nsi

if errorlevel 1 (
    echo Error: Installer creation failed.
    exit /b 1
)

echo Installer created: IKReverb-%VERSION%-Windows.exe
echo Build complete!