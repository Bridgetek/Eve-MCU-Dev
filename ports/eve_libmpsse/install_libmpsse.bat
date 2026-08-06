@echo off
:: See the README.md file in this directory for instructions

IF "%1"=="" goto installsetdir
:: FTDI distribution directory has been passed as an argument
set INSTALL_PATH=%1%
goto continueinstall

:installsetdir
:: Assume the imports directory from the FTDI distribution is extracted into the current directory
set INSTALL_PATH=%CD%\release

:continueinstall
IF NOT EXIST "%INSTALL_PATH%" goto notfound
IF NOT EXIST "%INSTALL_PATH%\libftd2xx" goto notfound
IF NOT EXIST "%INSTALL_PATH%\source" goto notfound
IF NOT EXIST "%INSTALL_PATH%\include" goto notfound

:: Find the processor architecture from the environment variables
IF /i "%PROCESSOR_ARCHITECTURE%"=="AMD64" goto installAMD64
IF /i "%PROCESSOR_ARCHITECTURE%"=="x86" goto installx86

echo Could not identify library to install. System reports "%PROCESSOR_ARCHITECTURE%".
echo AMD64 and x86 are supported.
goto commonexit

:installAMD64
echo Installing AMD64 libraries from "%1"
set INSTALL_MPSSE_PATH=build\x64\LIB\
set INSTALL_MPSSE_LIB=libmpsse.lib
goto commoncopy

:installx86
echo Installing x86 libraries from "%1"
set INSTALL_MPSSE_PATH=build\Win32\LIB\
set INSTALL_MPSSE_LIB=libmpsse.lib
goto commoncopy

:commoncopy
:: Copy the LIB and source files for libMPSSE and FTD2XX to the ports\eve_libmpsse directory
:: Use the same filenames as Linux (libmpsse and libftd2xx)

:: Copy the MPSSE LIB
echo Copying "%INSTALL_PATH%\%INSTALL_MPSSE_PATH%%INSTALL_MPSSE_LIB%" to %INSTALL_MPSSE_LIB%
copy /y "%INSTALL_PATH%\%INSTALL_MPSSE_PATH%\%INSTALL_MPSSE_LIB%" 

:: Copy the MPSSE  SPI headers
echo Copying "%INSTALL_PATH%\include\libmpsse_spi.h" to libmpsse_spi.h
copy /y "%INSTALL_PATH%\include\libmpsse_spi.h" 

:: Copy the SPI source code
echo Copying "%INSTALL_PATH%\source\ftdi_common.h" to ftdi_common.h
copy /y "%INSTALL_PATH%\source\ftdi_common.h"
echo Copying "%INSTALL_PATH%\source\ftdi_infra.c" to ftdi_infra.c
copy /y "%INSTALL_PATH%\source\ftdi_infra.c"
echo Copying "%INSTALL_PATH%\source\ftdi_infra.h" to ftdi_infra.h
copy /y "%INSTALL_PATH%\source\ftdi_infra.h"
echo Copying "%INSTALL_PATH%\source\ftdi_spi.c" to ftdi_spi.c
copy /y "%INSTALL_PATH%\source\ftdi_spi.c"
echo Copying "%INSTALL_PATH%\source\ftdi_mid.c" to ftdi_mid.c
copy /y "%INSTALL_PATH%\source\ftdi_mid.c"
echo Copying "%INSTALL_PATH%\source\ftdi_mid.h" to ftdi_mid.h
copy /y "%INSTALL_PATH%\source\ftdi_mid.h"
echo Copying "%INSTALL_PATH%\source\memcpy.c" to memcpy.c
copy /y "%INSTALL_PATH%\source\memcpy.c"

:: Copy the D2XX library header
echo Copying "%INSTALL_PATH%\libftd2xx\ftd2xx.h" to ftd2xx.h
copy /y "%INSTALL_PATH%\libftd2xx\ftd2xx.h" ftd2xx.h
:: NOTE: WinTypes.h is not required on Windows builds of the LiMPSSE library.
:: It is needed for Linux builds of the library but they copy the file to /usr/local/include
:: echo Copying "%INSTALL_PATH%\libftd2xx\WinTypes.h" to WinTypes.h
:: copy /y "%INSTALL_PATH%\libftd2xx\WinTypes.h" WinTypes.h

echo IMPORTANT:
echo Change line 246 on ftdi_infra.c:
echo  from 'hdll_d2xx = LoadLibrary(L"ftd2xx.dll");' to 'hdll_d2xx = LoadLibrary(TEXT("ftd2xx.dll"));'
echo Not doing this will result in "LoadLibrary failed: 126"

goto commonexit

:notfound
echo The distribution directory "%INSTALL_PATH%" was not found.

:commonexit
