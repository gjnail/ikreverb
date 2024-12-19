; IK Distortion Windows Installer Script
; Requires NSIS (Nullsoft Scriptable Install System)

; Include required libraries
!include "MUI2.nsh"
!include "x64.nsh"
!include "LogicLib.nsh"

; Plugin Information
!define PLUGIN_NAME "IKDistortion"
!define VERSION "1.0.0"
!define COMPANY "Internet Kids"
!define DESCRIPTION "Distortion Audio Plugin"
!define COPYRIGHT "Copyright © 2024 Internet Kids"

; Installer settings
SetCompressor /SOLID lzma
Name "${PLUGIN_NAME}"
OutFile "IKDistortion-${VERSION}-Windows.exe"
InstallDir "$PROGRAMFILES64\Common Files\VST3"
RequestExecutionLevel admin

; Modern UI
!include "MUI2.nsh"

; Interface Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Language
!insertmacro MUI_LANGUAGE "English"

; Installer sections
Section "VST3" SecVST3
    SetOutPath "$INSTDIR\${PLUGIN_NAME}.vst3"
    File /r "..\Builds\VisualStudio2022\x64\Release\VST3\${PLUGIN_NAME}.vst3\*.*"
    
    ; Write registry keys for VST3
    WriteRegStr HKLM "Software\${COMPANY}\${PLUGIN_NAME}" "VST3InstallLocation" "$INSTDIR\${PLUGIN_NAME}.vst3"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\${PLUGIN_NAME}.vst3\Uninstall.exe"
    
    ; Add uninstall information to Add/Remove Programs
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "DisplayName" "${PLUGIN_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "UninstallString" "$INSTDIR\${PLUGIN_NAME}.vst3\Uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "DisplayIcon" "$INSTDIR\${PLUGIN_NAME}.vst3\Uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "Publisher" "${COMPANY}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "DisplayVersion" "${VERSION}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "NoRepair" 1
SectionEnd

; Uninstaller section
Section "Uninstall"
    ; Remove VST3 plugin
    RMDir /r "$INSTDIR\${PLUGIN_NAME}.vst3"
    
    ; Remove registry keys
    DeleteRegKey HKLM "Software\${COMPANY}\${PLUGIN_NAME}"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}"
SectionEnd

; Descriptions
LangString DESC_SecVST3 ${LANG_ENGLISH} "Install VST3 plugin"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecVST3} $(DESC_SecVST3)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; Ensure proper Windows version and architecture
Function .onInit
    ${If} ${RunningX64}
        SetRegView 64
    ${Else}
        MessageBox MB_OK|MB_ICONSTOP "This installer is for 64-bit Windows only."
        Abort
    ${EndIf}
FunctionEnd