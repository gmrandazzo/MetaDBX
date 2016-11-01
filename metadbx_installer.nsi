 ;--------------------------------
!include "MUI.nsh"

!define MUI_PRODUCT "MetaDBX 1.0.0"
!define MUI_FILE "metadbx"
!define MUI_VERSION "1.0.0"
!define MUI_BRANDINGTEXT "MetaDBX 1.0.0"
CRCCheck On
 
 
; The name of the installer - used throughout the installer
Name "MetaDBX"

; The file to write - where to put the finished installer
OutFile "MetaDBXInstall.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"

;--------------------------------
;Interface Settings
;--------------------------------
!define MUI_ABORTWARNING

;--------------------------------
;Pages
;--------------------------------

; Generic welcome screen (optional)
!insertmacro MUI_PAGE_WELCOME

; Custom welcome screen (optional)
!define MUI_WELCOMEPAGE_TITLE "Please Note:"
  !define MUI_WELCOMEPAGE_TEXT "This software is property of Giuseppe Marco Randazzo and is distributed under license LGPL version 3.\n\n\nThe author Giuseppe Marco Randazzo accepts no responsability for any mathematical, scientific, tecnical errors or limitations of the program.\n"
!insertmacro MUI_PAGE_WELCOME

; Installation directory dialog
!insertmacro MUI_PAGE_DIRECTORY

; Installation
!insertmacro MUI_PAGE_INSTFILES


; Readme (optional)
;!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\your_readme_file.txt"

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
;--------------------------------
!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; The stuff to install
;--------------------------------
Section "Install"

;Add files
  SetOutPath "$INSTDIR"
  File "${MUI_FILE}.exe"
  ;File app.ico
  File metadbx.exe
  File libgcc_s_dw2-1.dll
  File libstdc++-6.dll
  File QtCore4.dll
  File QtGui4.dll
  File QtXml4.dll

;create desktop shortcut
  CreateShortCut "$DESKTOP\${MUI_PRODUCT}.lnk" "$INSTDIR\${MUI_FILE}.exe" "" "$INSTDIR\${MUI_FILE}.ico"

 
;create start-menu items
  CreateDirectory "$SMPROGRAMS\${MUI_PRODUCT}"
  CreateShortCut "$SMPROGRAMS\${MUI_PRODUCT}\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\${MUI_PRODUCT}\${MUI_PRODUCT}.lnk" "$INSTDIR\${MUI_FILE}.exe" "" "$INSTDIR\${MUI_FILE}.exe" 0
 
;write uninstall information to the registry
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayName" "${MUI_PRODUCT} (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "UninstallString" "$INSTDIR\Uninstall.exe"
 
  WriteUninstaller "$INSTDIR\Uninstall.exe"
 
SectionEnd

;--------------------------------    
;Uninstaller Section  
Section "Uninstall"
 
;Delete Files 
  RMDir /r "$INSTDIR\*.*"    
 
;Remove the installation directory
  RMDir "$INSTDIR"
 
;Delete Start Menu Shortcuts
  Delete "$DESKTOP\${MUI_PRODUCT}.lnk"
  Delete "$SMPROGRAMS\${MUI_PRODUCT}\*.*"
  RmDir  "$SMPROGRAMS\${MUI_PRODUCT}"
 
;Delete Uninstaller And Unistall Registry Entries
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\${MUI_PRODUCT}"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}"  
 
SectionEnd
 
 
;--------------------------------    
;MessageBox Section
 
 
;Function that calls a messagebox when installation finished correctly
Function .onInstSuccess
  MessageBox MB_OK "You have successfully installed ${MUI_PRODUCT}. Use the desktop icon to start the program."
FunctionEnd
 
 
Function un.onUninstSuccess
  MessageBox MB_OK "You have successfully uninstalled ${MUI_PRODUCT}."
FunctionEnd
 
 
;eof
