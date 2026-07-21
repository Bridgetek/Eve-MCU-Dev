@echo off
:: See the README.md file in this directory for instructions

IF "%1"=="" goto installsetdir
:: EVE_Emulator distribution directory has been passed as an argument
set INSTALL_PATH=%1%
goto continueinstall

:installsetdir
:: Assume the EVE_Emulator distribution is extracted into the current directory
set INSTALL_PATH=%CD%\EVE_Emulator

:continueinstall
IF NOT EXIST "%INSTALL_PATH%" goto notfound
IF NOT EXIST "%INSTALL_PATH%\lib" goto notfound
IF NOT EXIST "%INSTALL_PATH%\bin" goto notfound
IF NOT EXIST "%INSTALL_PATH%\include" goto notfound

:: Find the processor architecture from the environment variables
IF /i "%PROCESSOR_ARCHITECTURE%"=="AMD64" goto installAMD64
IF /i "%PROCESSOR_ARCHITECTURE%"=="x86" goto notsupported
IF /i "%PROCESSOR_ARCHITECTURE%"=="ARM64" goto notsupported

echo Could not identify library to install. System reports "%PROCESSOR_ARCHITECTURE%".
echo AMD64, x86 and ARM64 are supported.
goto commonexit

:installAMD64
echo Installing AMD64 libraries from "%1"
goto commoncopy

:commoncopy
:: Copy the Emulator DLL, LIB, and header files to the ports\eve_emulator directory

echo Copying "%INSTALL_PATH%\bin\bt8xxemu.dll" to "bt8xxemu.dll"
copy /y "%INSTALL_PATH%\bin\bt8xxemu.dll" "bt8xxemu.dll"
echo Copying "%INSTALL_PATH%\lib\bt8xxemu.lib" to "bt8xxemu.lib"
copy /y "%INSTALL_PATH%\lib\bt8xxemu.lib" "bt8xxemu.lib"
echo Copying "%INSTALL_PATH%\bin\mx25lemu.dll" to "mx25lemu.dll"
copy /y "%INSTALL_PATH%\bin\mx25lemu.dll" "mx25lemu.dll"
echo Copying "%INSTALL_PATH%\bin\zlib.dll" to "zlib.dll"
copy /y "%INSTALL_PATH%\bin\zlib.dll" "zlib.dll"
echo Copying "%INSTALL_PATH%\include\bt8xxemu_inttypes.h" to "bt8xxemu_inttypes.h"
copy /y "%INSTALL_PATH%\include\bt8xxemu_inttypes.h" "bt8xxemu_inttypes.h"
echo Copying "%INSTALL_PATH%\include\bt8xxemu.h" to "bt8xxemu.h"
copy /y "%INSTALL_PATH%\include\bt8xxemu.h" "bt8xxemu.h"

goto commonexit

:notfound
echo The EVE_Emulator repository directory "%INSTALL_PATH%" was not found.

:notsupported
echo The EVE_Emulator library files do not support x86 or ARM64.

:commonexit
