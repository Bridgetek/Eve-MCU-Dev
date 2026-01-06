@echo off
:: See the README.md file in this directory for instructions

IF "%1"=="" goto installsetdir
:: FTDI distribution directory has been passed as an argument
set INSTALL_PATH=%1%
goto continueinstall

:installsetdir
:: Assume the imports directory from the FTDI distribution is extracted into the current directory
set INSTALL_PATH=%CD%\imports

:continueinstall
IF NOT EXIST "%INSTALL_PATH%" goto notfound

:: Find the processor architecture from the environment variables
IF /i "%PROCESSOR_ARCHITECTURE%"=="AMD64" goto installAMD64
IF /i "%PROCESSOR_ARCHITECTURE%"=="x86" goto installx86
IF /i "%PROCESSOR_ARCHITECTURE%"=="ARM64" goto installARM64

echo Could not identify library to install. System reports "%PROCESSOR_ARCHITECTURE%".
echo AMD64, x86 and ARM64 are supported.
goto commonexit

:installAMD64
echo Installing AMD64 libraries from "%1"
set INSTALL_PATH_FT4222=LibFT4222\dll\amd64
set INSTALL_PATH_FTD2XX=ftd2xx\dll\amd64
set INSTALL_FT4222_DLL=LibFT4222-64.dll
goto commoncopy

:installx86
echo Installing x86 libraries from "%1"
set INSTALL_PATH_FT4222=LibFT4222\dll\x86
set INSTALL_PATH_FTD2XX=ftd2xx\dll\x86
set INSTALL_FT4222_DLL=LibFT4222.dll
goto commoncopy

:installARM64
echo Installing ARM64 libraries from "%1"
set INSTALL_PATH_FT4222=LibFT4222\dll\arm64
set INSTALL_PATH_FTD2XX=ftd2xx\dll\arm64
set INSTALL_FT4222_DLL=LibFT4222-64.dll
goto commoncopy

:commoncopy
:: Copy the FT4222 DLL and LIB files for FT4222 and FTD2XX to the ports\eve_libft4222 directory
:: Use the same filenames as Linux (libft4222 and libftd2xx)

:: Copy the FT4222 DLL to the 
echo Copying "%INSTALL_PATH%\%INSTALL_PATH_FT4222%\%INSTALL_FT4222_DLL%" to %INSTALL_FT4222_DLL%
copy /y "%INSTALL_PATH%\%INSTALL_PATH_FT4222%\%INSTALL_FT4222_DLL%" 
:: Copy the FT4222 library to a common file name
echo Copying "%INSTALL_PATH%\%INSTALL_PATH_FT4222%\LibFT4222-64.lib" to libft4222.lib
copy /y "%INSTALL_PATH%\%INSTALL_PATH_FT4222%\LibFT4222-64.lib" libft4222.lib
:: Copy the FT4222 library header to a common file name
echo Copying "%INSTALL_PATH%\LibFT4222\inc\LibFT4222.h" to libft4222.h
copy /y "%INSTALL_PATH%\LibFT4222\inc\LibFT4222.h" libft4222.h

:: Copy the D2XX library to access the ftd2xx.dll file installed with the device driver
echo Copying "%INSTALL_PATH%\%INSTALL_PATH_FTD2XX%\ftd2xx.lib" to libftd2xx.lib
copy /y "%INSTALL_PATH%\%INSTALL_PATH_FTD2XX%\ftd2xx.lib" libftd2xx.lib
:: Copy the D2XX library header
echo Copying "%INSTALL_PATH%\ftd2xx\ftd2xx.h" to ftd2xx.h
copy /y "%INSTALL_PATH%\ftd2xx\ftd2xx.h" ftd2xx.h
echo Copying "%INSTALL_PATH%\ftd2xx\WinTypes.h" to WinTypes.h
copy /y "%INSTALL_PATH%\ftd2xx\WinTypes.h" WinTypes.h

goto commonexit

:notfound
echo The distribution directory "%INSTALL_PATH%" was not found.

:commonexit
