
echo Setting default EPICS ports...
set  EPICS_CA_SERVER_PORT=6064
echo EPICS_CA_SERVER_PORT=%EPICS_CA_SERVER_PORT%
set  EPICS_CA_REPEATER_PORT=6065
echo EPICS_CA_REPEATER_PORT=%EPICS_CA_REPEATER_PORT%

REM W:\work\R3.14.11\support\electronAnalyser\bin\win32-x86\electronAnalyser.exe W:\work\R3.14.11\support\electronAnalyser\bin\win32-x86\stelectronAnalyser.boot
C:\Projects\electronAnalyser\bin\win32-x86\electronAnalyser.exe C:\Projects\electronAnalyser\bin\win32-x86\stelectronAnalyser.boot

