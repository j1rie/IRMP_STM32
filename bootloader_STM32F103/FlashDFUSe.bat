@echo off
set filename=%1%
set FLASHTOOL=dfu-util.exe

:loop
 %FLASHTOOL% -d 1eaf:0003 -a 1 -D %filename% && goto :end

  call :sleep 2
  goto :loop

:sleep
  ping -n %1 localhost >nul
  goto :EOF

:end
