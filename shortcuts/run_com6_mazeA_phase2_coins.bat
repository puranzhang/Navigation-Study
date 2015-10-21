START /WAIT start_com6.bat_Shortcut.lnk
START /WAIT "Game" "C:UDK\UDK-2013-09\Binaries\Win32\UDK" "C:\UDK\UDK-2013-09\UDKGame\Content\Maps\FSB_Spring_2015\mazeA_phase2_coins.udk?game=mymod.FeelSpaceGame"


:LOOP
tasklist /FI "IMAGENAME eq UDK.exe" 2>NUL | find ":">NUL
IF "%ERRORLEVEL%"=="0" (
  (GOTO CONTINUE)
) ELSE (
  ECHO UDK is still running
  ping -n 1 127.0.0.1 > nul
  (GOTO LOOP)
)


:CONTINUE
taskkill /im start_arg.exe
START stop_com6.bat_Shortcut.lnk
