cd "c:\ElectronAnalyzerIOC"

@rem copy the build files from the W: drive to the local drive
set _shareddrive=T:\i09\IOC\BL09I-EA-IOC-05
xcopy /Y /E /V /I %_shareddrive%\bin c:\ElectronAnalyzerIOC\bin
xcopy /Y /E /V /I %_shareddrive%\dbd c:\ElectronAnalyzerIOC\dbd
xcopy /Y /E /V /I %_shareddrive%\db c:\ElectronAnalyzerIOC\db

@rem set the various environment options: path and EPICS stuff
call "bin\win32-x86\profile.bat"

echo
echo "Starting the Electron Analyser IOC"
@rem run up the IOC with the startup script as argument
bin\win32-x86\electronAnalyser.exe bin\win32-x86\stelectronAnalyser.boot

