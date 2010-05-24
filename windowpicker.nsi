; The name of the installer
Name "windowpicker"

; The file to write
OutFile "windowpicker.exe"

; The default installation directory
InstallDir $PROGRAMFILES\windowpicker

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\windowpicker" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

;!define MINGW "C:\Qt4\qt4-mingw-2010.02\mingw"
!define MINGW "C:\Qt\2010.02\mingw"
!define STARTUP_RUN_KEY				"SOFTWARE\Microsoft\Windows\CurrentVersion\Run"
!define WNDTITLE "windowpicker"
!define WNDCLASS "QPopup"

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles


;--------------------------------

; The stuff to install
Section "Program Files"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  
  ; Put file there
  File "install\windowpicker.exe"
  File "${MINGW}\bin\libgcc_s_dw2-1.dll"
  File "${MINGW}\bin\mingwm10.dll"
  File "AUTHORS"
  File "COPYING"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\windowpicker "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\windowpicker" "DisplayName" "windowpicker"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\windowpicker" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\windowpicker" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\windowpicker" "NoRepair" 1

  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\windowpicker"
  CreateShortCut "$SMPROGRAMS\windowpicker\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\windowpicker\windowpicker.lnk" "$INSTDIR\windowpicker.exe" "" "$INSTDIR\windowpicker.exe" 0
  
SectionEnd

Section "Run Program On Windows Startup"
	WriteRegStr HKLM "${STARTUP_RUN_KEY}" "windowpicker" "$INSTDIR\windowpicker.exe"
SectionEnd


;---------------------------------
;Run check

!macro RunCheckMacro UN
Function ${UN}RunCheck
retry:
FindWindow $0 "${WNDCLASS}" "${WNDTITLE}"
StrCmp $0 0 continue
MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "The application you are trying to install/remove is running. Close it and try again." /SD IDCANCEL IDRETRY retry
Abort
continue:
FunctionEnd
!macroend
!insertmacro RunCheckMacro ""
!insertmacro RunCheckMacro "un."

;--------------------------------

Function .onInit

  Call RunCheck

FunctionEnd

;--------------------------------

; Uninstaller

;--------------------------------------------------------------------------------

 
Function un.onInit

  Call un.RunCheck
	;MessageBox MB_YESNO "This will uninstall. Continue?" IDYES NoAbort
    ;  Abort ; causes uninstaller to quit.
    ;NoAbort:

FunctionEnd

Section "Uninstall"
;	Call un.onInit
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\windowpicker"
  DeleteRegKey HKLM SOFTWARE\windowpicker

  ; Remove files and uninstaller
  Delete $INSTDIR\windowpicker.exe
  Delete $INSTDIR\libgcc_s_dw2-1.dll
  Delete $INSTDIR\mingwm10.dll
  Delete $INSTDIR\AUTHORS
  Delete $INSTDIR\COPYING

  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\windowpicker\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\windowpicker"
  RMDir "$INSTDIR"

 ; Remove Registry Entries 
	DeleteRegValue HKLM "${STARTUP_RUN_KEY}" "windowpicker"

SectionEnd

