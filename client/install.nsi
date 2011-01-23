Name "RepatriatorClient"
OutFile "RepatriatorClientSetup.exe"

InstallDir "$PROGRAMFILES\RepatriatorClient"
InstallDirRegKey HKLM "Software\RepatriatorClient" "Install_Dir"
RequestExecutionLevel admin 

;------------------------------


; Pages

Page directory
Page instfiles

UninstPage instfiles

;-------------------------------

; stuff to install
Section "RepatriatorClient (required)"
    SectionIn RO

    SetOutPath $INSTDIR

    ; Files to install
    File "RepatriatorClient.exe"
    File "libgcc_s_dw2-1.dll"
    File "mingwm10.dll"
    File "QtCore4.dll"
    File "QtGui4.dll"
    File "QtNetwork4.dll"
    CreateDirectory "$INSTDIR\imageformats"
    File "/oname=imageformats\qjpeg4.dll" "imageformats\qjpeg4.dll"

    ; write the installation path into the registry
    WriteRegStr HKLM SOFTWARE\RepatriatorClient "Install_Dir" "$INSTDIR"

    ; write the uninstall keys for Windows
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RepatriatorClient" "DisplayName" "RepatriatorClient"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RepatriatorClient" "UninstallString" '"$INSTDIR\uninstall.exe"'
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RepatriatorClient" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RepatriatorClient" "NoRepair" 1
    WriteUninstaller "uninstall.exe"
SectionEnd

Section "Shortcuts"
    CreateDirectory "$SMPROGRAMS\RepatriatorClient"
    CreateShortCut "$SMPROGRAMS\RepatriatorClient\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
    CreateShortCut "$SMPROGRAMS\RepatriatorClient\RepatriatorClient.lnk" "$INSTDIR\RepatriatorClient.exe" "" "$INSTDIR\RepatriatorClient.exe" 0
    CreateShortCut "$DESKTOP\RepatriatorClient.lnk" "$INSTDIR\RepatriatorClient.exe" "" "$INSTDIR\RepatriatorClient.exe" 0
SectionEnd

;---------------------

; Uninstaller

Section "Uninstall"
    ; Remove registry keys
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\RepatriatorClient"
    DeleteRegKey HKLM SOFTWARE\RepatriatorClient

    ; Remove files and uninstaller
    Delete "$INSTDIR\*.*"
    Delete "$INSTDIR\imageformats\*.*"
    RMDir "$INSTDIR\imageformats"

    ; Remove shortcuts, if any
    Delete "$SMPROGRAMS\RepatriatorClient\*.*"
    Delete "$DESKTOP\RepatriatorClient.lnk"

    ; Remove directories used
    RMDir "$SMPROGRAMS\RepatriatorClient"
    RMDir "$INSTDIR"
SectionEnd
