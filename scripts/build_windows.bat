@echo off
setlocal enabledelayedexpansion

:: Configuration
set PLUGIN_NAME=IKDistortion
set VERSION=1.0.0

:: Check for Visual Studio installation
where /q MSBuild.exe
if ERRORLEVEL 1 (
    echo MSBuild not found. Please run this script from a Visual Studio Developer Command Prompt.
    exit /b 1
)

:: Check for NSIS installation
where /q makensis.exe
if ERRORLEVEL 1 (
    echo NSIS not found. Please install NSIS from https://nsis.sourceforge.io/Download
    exit /b 1
)

:: Build the plugin
echo Building plugin...
MSBuild.exe ..\Builds\VisualStudio2022\%PLUGIN_NAME%.sln /p:Configuration=Release /p:Platform=x64

if ERRORLEVEL 1 (
    echo Build failed
    exit /b 1
)

:: Create installer
echo Creating installer...
makensis.exe windows_installer.nsi

if ERRORLEVEL 1 (
    echo Installer creation failed
    exit /b 1
)

echo Build completed successfully!
echo Installer created: IKDistortion-%VERSION%-Windows.exe 