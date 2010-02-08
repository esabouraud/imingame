# Auto-generated by EclipseNSIS Script Wizard
# 29 juil. 2009 15:13:10

Name IMinGame

# General Symbol Definitions
!define REGKEY "SOFTWARE\$(^Name)"
!define REGROOT "SHELL_CONTEXT" 
!define VERSION 0.2.7
!define COMPANY "Eric Sabouraud"
!define URL http://sourceforge.net/projects/imingame/

# Persistent language selection
!define MUI_LANGDLL_REGISTRY_ROOT "${REGROOT}"
!define MUI_LANGDLL_REGISTRY_KEY "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"
 
# MultiUser Symbol Definitions
!define MULTIUSER_EXECUTIONLEVEL Power
!define MULTIUSER_INSTALLMODE_DEFAULT_CURRENTUSER
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY "${REGKEY}"
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME MultiUserInstallMode
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!define MULTIUSER_INSTALLMODE_INSTDIR IMinGame
!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_KEY "${REGKEY}"
!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_VALUE "Path"

# MUI Symbol Definitions
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install-colorful.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "IMinGame\res\iig_welcome.bmp"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${REGROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER IMinGame
!define MUI_FINISHPAGE_RUN $INSTDIR\IMinGame.exe
!define MUI_FINISHPAGE_SHOWREADME $(readmeStr)
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall-colorful.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include MultiUser.nsh
!include Sections.nsh
!include MUI2.nsh

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE $(licenseStr)
!insertmacro MULTIUSER_PAGE_INSTALLMODE
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English
!insertmacro MUI_LANGUAGE French
#!insertmacro MUI_LANGUAGE Spanish
#!insertmacro MUI_LANGUAGE German

# Installer Language Strings
LicenseLangString licenseStr ${LANG_ENGLISH} "LICENSE-eng.txt"
LicenseLangString licenseStr ${LANG_FRENCH}  "LICENSE-fra.txt"
#LicenseLangString licenseStr ${LANG_SPANISH} LICENSE-esp.txt
#LicenseLangString licenseStr ${LANG_GERMAN}  LICENSE-deu.txt

LangString readmeStr ${LANG_ENGLISH} "README-eng.txt"
LangString readmeStr ${LANG_FRENCH}  "README-fra.txt"
#LicenseLangString readmeStr ${LANG_SPANISH} README-esp.txt
#LicenseLangString readmeStr ${LANG_GERMAN}  README-deu.txt

LangString ^UninstallLink ${LANG_ENGLISH} "Uninstall $(^Name)"
LangString ^UninstallLink ${LANG_FRENCH}  "Désinstaller $(^Name)"
#LangString ^UninstallLink ${LANG_SPANISH} "Desinstale $(^Name)"
#LangString ^UninstallLink ${LANG_GERMAN}  "Deinstallieren  $(^Name)"

LangString PresetLang ${LANG_ENGLISH} "0"
LangString PresetLang ${LANG_FRENCH}  "1"
#LangString PresetLang ${LANG_SPANISH} "2"
#LangString PresetLang ${LANG_GERMAN}  "3"

LangString KeepPrefs ${LANG_ENGLISH} "Keep preferences ?"
LangString KeepPrefs ${LANG_FRENCH}  "Garder la configuration ?"
#LangString KeepPrefs ${LANG_SPANISH} "Keep preferences ?"
#LangString KeepPrefs ${LANG_GERMAN}  "Keep preferences ?"

LangString NoRuntime ${LANG_ENGLISH} "Download and install Microsoft Visual C++ 2008 SP1 Redistributable Package (x86) (needed for IMinGame) ?"
LangString NoRuntime ${LANG_FRENCH} "Télécharger et installer Microsoft Visual C++ 2008 SP1 Redistributable Package (x86) (nécessaire pour IMinGame) ?"

LangString CoreFiles ${LANG_ENGLISH} "Core files"
LangString CoreFiles ${LANG_FRENCH} "Fichiers principaux"

LangString RunStartup ${LANG_ENGLISH} "Run on start"
LangString RunStartup ${LANG_FRENCH} "Lancer au démarrage"

# Installer attributes
OutFile imingame-${VERSION}-setup.exe
InstallDir IMinGame
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion ${VERSION}.0
VIAddVersionKey /LANG=${LANG_ENGLISH} ProductName "IMinGame"
VIAddVersionKey /LANG=${LANG_ENGLISH} ProductVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} CompanyName "${COMPANY}"
VIAddVersionKey /LANG=${LANG_ENGLISH} CompanyWebsite "${URL}"
VIAddVersionKey /LANG=${LANG_ENGLISH} FileVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} FileDescription ""
VIAddVersionKey /LANG=${LANG_ENGLISH} LegalCopyright ""
#InstallDirRegKey ${REGROOT} "${REGKEY}" Path
ShowUninstDetails show

# Installer sections
Section "!$(CoreFiles)" SEC0000
    SetOutPath $INSTDIR
    SetOverwrite on
    File Release\IMinGameHook.dll
    File Release\IMinGame.exe
    File README-eng.txt
    File README-fra.txt
	File CHANGELOG.txt
    File LICENSE-eng.txt
	File LICENSE-fra.txt
	File TODO.txt
    WriteRegStr ${REGROOT}  "${REGKEY}\Components" Main 1
	WriteINIStr $INSTDIR\presettings.ini "general" lang $(PresetLang)

SectionEnd

Section -post SEC0001
    WriteRegStr ${REGROOT}  "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuGroup"
    SetOutPath $INSTDIR
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^Name).lnk" $INSTDIR\IMinGame.exe
	CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Readme.lnk" $INSTDIR\$(readmeStr)
    SetOutPath $SMPROGRAMS\$StartMenuGroup
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk" $INSTDIR\uninstall.exe "/$MultiUser.InstallMode"
    !insertmacro MUI_STARTMENU_WRITE_END
	SetOutPath $INSTDIR
    WriteRegStr ${REGROOT}  "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr ${REGROOT}  "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr ${REGROOT}  "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr ${REGROOT}  "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr ${REGROOT}  "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr ${REGROOT}  "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString "$INSTDIR\uninstall.exe /$MultiUser.InstallMode"
    WriteRegDWORD ${REGROOT}  "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD ${REGROOT}  "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
	Call CheckVisualRuntime
SectionEnd

Section /o $(RunStartup) SEC0002
	WriteRegStr ${REGROOT}  "${REGKEY}\Components" RunStartup 1
	WriteRegStr ${REGROOT}  "Software\Microsoft\Windows\CurrentVersion\Run" "IMinGame" "$INSTDIR\IMinGame.exe /m"
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 ${REGROOT} "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o -un.Main UNSEC0000

	MessageBox MB_YESNO|MB_ICONQUESTION "$(KeepPrefs)" IDYES del_instfiles
	Delete /REBOOTOK $INSTDIR\settings.ini
	Delete /REBOOTOK $INSTDIR\wlist.txt
	Delete /REBOOTOK $INSTDIR\blist.txt
del_instfiles:	
	Delete /REBOOTOK $INSTDIR\presettings.ini
    Delete /REBOOTOK $INSTDIR\TODO.txt
	Delete /REBOOTOK $INSTDIR\LICENSE-fra.txt
    Delete /REBOOTOK $INSTDIR\LICENSE-eng.txt
    Delete /REBOOTOK $INSTDIR\CHANGELOG.txt
    Delete /REBOOTOK $INSTDIR\README-fra.txt
	Delete /REBOOTOK $INSTDIR\README-eng.txt
    Delete /REBOOTOK $INSTDIR\IMinGame.exe
    Delete /REBOOTOK $INSTDIR\IMinGameHook.dll
    DeleteRegValue ${REGROOT} "${REGKEY}\Components" Main
SectionEnd

Section -un.post UNSEC0001
    DeleteRegKey ${REGROOT} "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\$(^Name).lnk"
	Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\Readme.lnk"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue ${REGROOT} "${REGKEY}" StartMenuGroup
    DeleteRegValue ${REGROOT} "${REGKEY}" Path
    DeleteRegKey /IfEmpty ${REGROOT} "${REGKEY}\Components"
    DeleteRegKey /IfEmpty ${REGROOT} "${REGKEY}"
    RmDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RmDir /REBOOTOK $INSTDIR
    Push $R0
    StrCpy $R0 $StartMenuGroup 1
    StrCmp $R0 ">" no_smgroup
no_smgroup:
    Pop $R0
SectionEnd

Section /o un.RunStartup UNSEC0002
	DeleteRegValue ${REGROOT} "${REGKEY}\Components" RunStartup
	DeleteRegValue ${REGROOT} "Software\Microsoft\Windows\CurrentVersion\Run" "IMinGame"
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir
    !insertmacro MUI_LANGDLL_DISPLAY
    !insertmacro MULTIUSER_INIT
FunctionEnd


Function CheckVisualRuntime
	Push $R0
	Push $R1
	FindFirst $R1 $R0 "$WINDIR\WinSxS\x86_Microsoft.VC90.CRT_1fc8b3b9a1e18e3b_9.0.30729.1*"
	IfErrors 0 runtime_present
		MessageBox MB_YESNO|MB_ICONQUESTION  "$(NoRuntime)" IDNO runtime_present
		ExecShell Open "http://download.microsoft.com/download/d/d/9/dd9a82d0-52ef-40db-8dab-795376989c03/vcredist_x86.exe" SW_SHOWNORMAL
runtime_present:
	FindClose $R1
	Pop $R1
	Pop $R0
FunctionEnd


# Uninstaller functions
Function un.onInit
    !insertmacro MULTIUSER_UNINIT
	!insertmacro MUI_UNGETLANGUAGE
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION Main ${UNSEC0000}
	!insertmacro SELECT_UNSECTION RunStartup ${UNSEC0002}
FunctionEnd

