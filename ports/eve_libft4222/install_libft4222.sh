# See the README.md file in this directory for instructions

if [[ $1 != "" ]]; then
    # FTDI distribution directory has been passed as an argument
    INSTALL_PATH="$1"
else
    # Assume the imports directory from the FTDI distribution is extracted into the current directory
    INSTALL_PATH="$(pwd)/imports"
fi

if [[ ! -e "$INSTALL_PATH" ]]; then
    echo The distribution directory "$INSTALL_PATH" was not found.
    exit
fi

if [[ ! -e "$INSTALL_PATH/LibFT4222" || ! -e "$INSTALL_PATH/ftd2xx" ]]; then
    echo The distribution directory "$INSTALL_PATH" is not right.
    exit
fi

if [[ $PROCESSOR_ARCHITECTURE == "" ]]; then
    case $(uname -m) in
        i386)   PROCESSOR_ARCHITECTURE="x86" ;;
        i686)   PROCESSOR_ARCHITECTURE="x86" ;;
        x86_64) PROCESSOR_ARCHITECTURE="AMD64" ;;
        arm)    PROCESSOR_ARCHITECTURE="ARM64" ;;
    esac
fi

# Find the processor architecture from the environment variables
if [[ $PROCESSOR_ARCHITECTURE == "AMD64" ]]; then
    echo Installing AMD64 libraries from "$1"
    INSTALL_PATH_FT4222=LibFT4222/dll/amd64
    INSTALL_PATH_FTD2XX=ftd2xx/dll/amd64
    INSTALL_FT4222_DLL=LibFT4222-64.dll
elif [[ $PROCESSOR_ARCHITECTURE == "x86" ]]; then
    echo Installing x86 libraries from "$1"
    INSTALL_PATH_FT4222=LibFT4222/dll/x86
    INSTALL_PATH_FTD2XX=ftd2xx/dll/x86
    INSTALL_FT4222_DLL=LibFT4222.dll
elif [[ $PROCESSOR_ARCHITECTURE == "ARM64" ]]; then
    echo Installing ARM64 libraries from "$1"
    INSTALL_PATH_FT4222=LibFT4222/dll/arm64
    INSTALL_PATH_FTD2XX=ftd2xx/dll/arm64
    INSTALL_FT4222_DLL=LibFT4222-64.dll
else
    echo Could not identify library to install. System reports "$PROCESSOR_ARCHITECTURE".
    echo AMD64, x86 and ARM64 are supported.
    exit
fi

# Copy the FT4222 DLL and LIB files for FT4222 and FTD2XX to the ports/eve_libft4222 directory
# Use the same filenames as Linux (libft4222 and libftd2xx)

# Copy the FT4222 DLL to the 
echo Copying "$INSTALL_PATH/$INSTALL_PATH_FT4222/$INSTALL_FT4222_DLL" to $INSTALL_FT4222_DLL
cp "$INSTALL_PATH/$INSTALL_PATH_FT4222/$INSTALL_FT4222_DLL" $INSTALL_FT4222_DLL
# Copy the FT4222 library to a common file name
echo Copying "$INSTALL_PATH/$INSTALL_PATH_FT4222/LibFT4222-64.lib" to libft4222.lib
cp "$INSTALL_PATH/$INSTALL_PATH_FT4222/LibFT4222-64.lib" libft4222.lib
# Copy the FT4222 library header to a common file name
echo Copying "$INSTALL_PATH/LibFT4222/inc/LibFT4222.h" to libft4222.h
cp "$INSTALL_PATH/LibFT4222/inc/LibFT4222.h" libft4222.h

# Copy the D2XX library to access the ftd2xx.dll file installed with the device driver
echo Copying "$INSTALL_PATH/$INSTALL_PATH_FTD2XX/ftd2xx.lib" to libftd2xx.lib
cp "$INSTALL_PATH/$INSTALL_PATH_FTD2XX/ftd2xx.lib" libftd2xx.lib
# Copy the D2XX library header
echo Copying "$INSTALL_PATH/ftd2xx/ftd2xx.h" to ftd2xx.h
cp "$INSTALL_PATH/ftd2xx/ftd2xx.h" ftd2xx.h
echo Copying "$INSTALL_PATH/ftd2xx/WinTypes.h" to WinTypes.h
cp "$INSTALL_PATH/ftd2xx/WinTypes.h" WinTypes.h
