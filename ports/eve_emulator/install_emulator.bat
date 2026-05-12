@echo off
:: See the README.md file in this directory for instructions

IF "%1"=="" goto installsetdir
:: FTDI distribution directory has been passed as an argument
set INSTALL_PATH=%1%
goto continueinstall

:installsetdir
:: Assume the imports directory from the FTDI distribution is extracted into the current directory
set INSTALL_PATH=%CD%\EveApps

:continueinstall
IF NOT EXIST "%INSTALL_PATH%" goto notfound
IF NOT EXIST "%INSTALL_PATH%\common\eve_hal" goto notfound
IF NOT EXIST "%INSTALL_PATH%\common/eve_hal\Bin\Simulation" goto notfound
IF NOT EXIST "%INSTALL_PATH%\common/eve_hal\Hdr" goto notfound

:: Find the processor architecture from the environment variables
IF /i "%PROCESSOR_ARCHITECTURE%"=="AMD64" goto installAMD64
IF /i "%PROCESSOR_ARCHITECTURE%"=="x86" goto installx86
IF /i "%PROCESSOR_ARCHITECTURE%"=="ARM64" goto installARM64

echo Could not identify library to install. System reports "%PROCESSOR_ARCHITECTURE%".
echo AMD64, x86 and ARM64 are supported.
goto commonexit

:installAMD64
echo Installing AMD64 libraries from "%1"
set INSTALL_PATH_EMULATOR=common\eve_hal\Bin\Simulation\x64
goto commoncopy

:installx86
echo Installing x86 libraries from "%1"
set INSTALL_PATH_EMULATOR=common\eve_hal\Bin\Simulation\x86
goto commoncopy

:installARM64
echo Installing ARM64 libraries from "%1"
set INSTALL_PATH_EMULATOR=common\eve_hal\Bin\Simulation\arm64
goto commoncopy

:commoncopy
:: Copy the Emulator DLL and LIB files to the ports\eve_emulator directory

echo Copying "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\bt8xxemu.dll" to "bt8xxemu.dll"
copy /y "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\bt8xxemu.dll" "bt8xxemu.dll"
echo Copying "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\bt8xxemu.lib" to "bt8xxemu.lib"
copy /y "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\bt8xxemu.lib" "bt8xxemu.lib"
echo Copying "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\mx25lemu.dll" to "mx25lemu.dll"
copy /y "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\mx25lemu.dll" "mx25lemu.dll"
echo Copying "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\mx25lemu.lib" to "mx25lemu.lib"
copy /y "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\mx25lemu.lib" "mx25lemu.lib"
:: echo Copying "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\ft8xxemu.dll" to "ft8xxemu.dll"
:: copy /y "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\ft8xxemu.dll" "ft8xxemu.dll"
:: echo Copying "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\ft8xxemu.lib" to "ft8xxemu.lib"
:: copy /y "%INSTALL_PATH%\%INSTALL_PATH_EMULATOR%\ft8xxemu.lib" "ft8xxemu.lib"

echo Copying "%INSTALL_PATH%\common\eve_hal\Hdr\bt8xxemu_inttypes.h" to "bt8xxemu_inttypes.h"
copy /y "%INSTALL_PATH%\common\eve_hal\Hdr\bt8xxemu_inttypes.h" "bt8xxemu_inttypes.h"
echo Copying "%INSTALL_PATH%\common\eve_hal\Hdr\bt8xxemu.h" to "bt8xxemu.h"
copy /y "%INSTALL_PATH%\common\eve_hal\Hdr\bt8xxemu.h" "bt8xxemu.h"

goto commonexit

:notfound
echo The EveApps repository directory "%INSTALL_PATH%" was not found.

:commonexit
