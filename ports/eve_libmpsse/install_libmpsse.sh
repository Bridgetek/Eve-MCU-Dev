#!/bin/bash
# See the README.md file in this directory for instructions

if [[ $1 != "" ]]; then
    # FTDI distribution directory has been passed as an argument
    INSTALL_PATH="$1"
else
    # Assume the imports directory from the FTDI distribution is extracted into the current directory
    INSTALL_PATH="$(pwd)/release"
fi

if [[ ! -e "$INSTALL_PATH" ]]; then
    echo The distribution directory "$INSTALL_PATH" was not found.
    exit
fi

if [[ ! -e "$INSTALL_PATH/libftd2xx" || ! -e "$INSTALL_PATH/source" || ! -e "$INSTALL_PATH/include"]]; then
    echo The distribution directory "$INSTALL_PATH" is not right.
    exit
fi

if [[ $PROCESSOR_ARCHITECTURE == "" ]]; then
    case $(uname -m) in
        i386)   PROCESSOR_ARCHITECTURE="x86" ;;
        i686)   PROCESSOR_ARCHITECTURE="x86" ;;
        x86_64) PROCESSOR_ARCHITECTURE="AMD64" ;;
    esac
fi

# Find the processor architecture from the environment variables
if [[ $PROCESSOR_ARCHITECTURE == "AMD64" ]]; then
    echo Installing AMD64 libraries from "$1"
    INSTALL_MPSSE_PATH=build/x64/LIB/
    INSTALL_MPSSE_LIB=libmpsse.lib
elif [[ $PROCESSOR_ARCHITECTURE == "x86" ]]; then
    echo Installing x86 libraries from "$1"
    INSTALL_MPSSE_PATH=build/Win32/LIB/
    INSTALL_MPSSE_LIB=libmpsse.lib
else
    echo Could not identify library to install. System reports "$PROCESSOR_ARCHITECTURE".
    echo AMD64 and x86 are supported.
    exit
fi

# Copy the MPSSE LIB and source files for libMPSSE and FTD2XX to the ports/eve_libmpsse directory
# Use the same filenames as Linux (libft4222 and libftd2xx)

# Copy the MPSSE LIB
echo Copying "$INSTALL_PATH/$INSTALL_MPSSE_PATH/$INSTALL_MPSSE_LIB" to $INSTALL_MPSSE_LIB
cp "$INSTALL_PATH/$INSTALL_MPSSE_PATH/$INSTALL_MPSSE_LIB" 

# Copy the MPSSE SPI headers
echo Copying "$INSTALL_PATH/include/libmpsse_spi.h" to libmpsse_spi.h
cp "$INSTALL_PATH/include/libmpsse_spi.h" 

# Copy the SPI source code
echo Copying "$INSTALL_PATH/source/ftdi_common.h" to ftdi_common.h
cp  "$INSTALL_PATH/source/ftdi_common.h"
echo Copying "$INSTALL_PATH/source/ftdi_infra.c" to ftdi_infra.c
cp "$INSTALL_PATH/source/ftdi_infra.c"
echo Copying "$INSTALL_PATH/source/ftdi_infra.h" to ftdi_infra.h
cpp  "$INSTALL_PATH/source/ftdi_infra.h"
echo Copying "$INSTALL_PATH/source/ftdi_spi.c" to ftdi_spi.c
cp "$INSTALL_PATH/source/ftdi_spi.c"
echo Copying "$INSTALL_PATH/source/ftdi_mid.c" to ftdi_mid.c
cp "$INSTALL_PATH/source/ftdi_mid.c"
echo Copying "$INSTALL_PATH/source/ftdi_mid.h" to ftdi_mid.h
cp "$INSTALL_PATH/source/ftdi_mid.h"
echo Copying "$INSTALL_PATH/source/memcpy.c" to memcpy.c
cp "$INSTALL_PATH/source/memcpy.c"

# Copy the D2XX library header
echo Copying "$INSTALL_PATH/libftd2xx/ftd2xx.h" to ftd2xx.h
cp "$INSTALL_PATH/libftd2xx/ftd2xx.h" ftd2xx.h
echo Copying "$INSTALL_PATH/libftd2xx/WinTypes.h" to WinTypes.h
cp "$INSTALL_PATH/libftd2xx/WinTypes.h" WinTypes.h

echo IMPORTANT:
echo Change line 246 on ftdi_infra.c:
echo  from 'hdll_d2xx = LoadLibrary(L"ftd2xx.dll");' to 'hdll_d2xx = LoadLibrary(TEXT("ftd2xx.dll"));'
echo Not doing this will result in "LoadLibrary failed: 126"
