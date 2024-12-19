; IK Reverb Windows Installer Script

!include "MUI2.nsh"
!include "FileFunc.nsh"

; Version info
!define VERSION "1.0.0"
!define COMPANY "Internet Kids"
!define URL "https://internetkidsmaketechno.com"
!define PLUGIN_NAME "IKReverb"
!define PLUGIN_DISPLAY_NAME "IK Reverb"
!define DESCRIPTION "Reverb Audio Plugin"

; Installer settings
SetCompressor /SOLID lzma
Name "${PLUGIN_DISPLAY_NAME}"
OutFile "IKReverb-${VERSION}-Windows.exe"
InstallDir "$PROGRAMFILES64\Common Files\VST3"
RequestExecutionLevel admin

; Interface settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Languages
!insertmacro MUI_LANGUAGE "English"

Section "Install"
    SetOutPath "$INSTDIR"
    File /r "..\Builds\VisualStudio2022\x64\Release\VST3\${PLUGIN_NAME}.vst3\"
    WriteUninstaller "$INSTDIR\Uninstall-${PLUGIN_NAME}.exe"
SectionEnd

Section "Uninstall"
    RMDir /r "$INSTDIR\${PLUGIN_NAME}.vst3"
    Delete "$INSTDIR\Uninstall-${PLUGIN_NAME}.exe"
SectionEnd