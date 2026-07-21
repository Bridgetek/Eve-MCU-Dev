# See the README.md file in this directory for instructions

if [[ $1 != "" ]]; then
    # EVE_Emulator repository directory has been passed as an argument
    INSTALL_PATH="$1"
else
    # Assume the EVE_Emulator repository is extracted into the current directory
    INSTALL_PATH="$(pwd)/EVE_Emulator"
fi

if [[ ! -e "$INSTALL_PATH" || ! -e "$INSTALL_PATH/lib" || ! -e "$INSTALL_PATH/include" || ! -e "$INSTALL_PATH/bin" ]]; then
    echo The EVE_Emulator repository directory "$INSTALL_PATH" was not found.
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
else
    echo Could not identify library to install. System reports "$PROCESSOR_ARCHITECTURE".
    echo Only AMD64 is supported.
    exit
fi

# Copy the Emulator DLL and LIB files to the ports\eve_emulator directory

echo Copying "$INSTALL_PATH/bin/bt8xxemu.dll" to "bt8xxemu.dll"
cp "$INSTALL_PATH/bin/bt8xxemu.dll" "bt8xxemu.dll"
echo Copying "$INSTALL_PATH/lib/bt8xxemu.lib" to "bt8xxemu.lib"
cp "$INSTALL_PATH/lib/bt8xxemu.lib" "bt8xxemu.lib"
echo Copying "$INSTALL_PATH/bin/mx25lemu.dll" to "mx25lemu.dll"
cp "$INSTALL_PATH/bin/mx25lemu.dll" "mx25lemu.dll"
echo Copying "$INSTALL_PATH/bin/zlib.dll" to "zlib.dll"
cp "$INSTALL_PATH/bin/zlib.dll" "zlib.dll"
echo Copying "$INSTALL_PATH/include/bt8xxemu_inttypes.h" to "bt8xxemu_inttypes.h"
cp "$INSTALL_PATH/include/bt8xxemu_inttypes.h" "bt8xxemu_inttypes.h"
echo Copying "$INSTALL_PATH/include/bt8xxemu.h" to "bt8xxemu.h"
cp "$INSTALL_PATH/include/bt8xxemu.h" "bt8xxemu.h"
