
echo Setting default EPICS ports...
set  EPICS_CA_SERVER_PORT=6064
echo EPICS_CA_SERVER_PORT=%EPICS_CA_SERVER_PORT%
set  EPICS_CA_REPEATER_PORT=6065
echo EPICS_CA_REPEATER_PORT=%EPICS_CA_REPEATER_PORT%
set  EPICS_CA_MAX_ARRAY_BYTES=100000000
echo EPICS_CA_MAX_ARRAY_BYTES=%EPICS_CA_MAX_ARRAY_BYTES%

REM C:\Projects\electronAnalyser\bin\win32-x86\electronAnalyser.exe C:\Projects\electronAnalyser\bin\win32-x86\stelectronAnalyser.boot
W:\work\R3.14.12.3\support\electronAnalyser\bin\win32-x86\electronAnalyser.exe W:\work\R3.14.12.3\support\electronAnalyser\bin\win32-x86\stelectronAnalyser.boot
