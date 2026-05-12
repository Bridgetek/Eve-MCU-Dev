# See the README.md file in this directory for instructions

if [[ $1 != "" ]]; then
    # EveApps repository directory has been passed as an argument
    INSTALL_PATH="$1"
else
    # Assume the EveApps repository is extracted into the current directory
    INSTALL_PATH="$(pwd)/EveApps"
fi

if [[ ! -e "$INSTALL_PATH" || ! -e "$INSTALL_PATH/LibFT4222" || ! -e "$INSTALL_PATH/ftd2xx" ]]; then
    echo The EveApps repository directory "$INSTALL_PATH" was not found.
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
    INSTALL_PATH_EMULATOR=common/eve_hal/Bin/Simulation/x64
elif [[ $PROCESSOR_ARCHITECTURE == "x86" ]]; then
    echo Installing x86 libraries from "$1"
    INSTALL_PATH_EMULATOR=common/eve_hal/Bin/Simulation/x86
elif [[ $PROCESSOR_ARCHITECTURE == "ARM64" ]]; then
    echo Installing ARM64 libraries from "$1"
    INSTALL_PATH_EMULATOR=common/eve_hal/Bin/Simulation/arm64
else
    echo Could not identify library to install. System reports "$PROCESSOR_ARCHITECTURE".
    echo AMD64, x86 and ARM64 are supported.
    exit
fi

# Copy the Emulator DLL and LIB files to the ports\eve_emulator directory

echo Copying "$INSTALL_PATH/$INSTALL_PATH_EMULATOR/bt8xxemu.dll" to "bt8xxemu.dll"
cp "$INSTALL_PATH/$INSTALL_PATH_EMULATOR/bt8xxemu.dll" "bt8xxemu.dll"
echo Copying "$INSTALL_PATH/$INSTALL_PATH_EMULATOR/bt8xxemu.lib" to "bt8xxemu.lib"
cp "$INSTALL_PATH/$INSTALL_PATH_EMULATOR/bt8xxemu.lib" "bt8xxemu.lib"
echo Copying "$INSTALL_PATH/$INSTALL_PATH_EMULATOR/mx25lemu.dll" to "mx25lemu.dll"
cp "$INSTALL_PATH/$INSTALL_PATH_EMULATOR/mx25lemu.dll" "mx25lemu.dll"
echo Copying "$INSTALL_PATH/$INSTALL_PATH_EMULATOR/mx25lemu.lib" to "mx25lemu.lib"
cp "$INSTALL_PATH/$INSTALL_PATH_EMULATOR/mx25lemu.lib" "mx25lemu.lib"

echo Copying "$INSTALL_PATH/common/eve_hal/Hdr/bt8xxemu_inttypes.h" to "bt8xxemu_inttypes.h"
cp "$INSTALL_PATH/common/eve_hal/Hdr/bt8xxemu_inttypes.h" "bt8xxemu_inttypes.h"
echo Copying "$INSTALL_PATH/common/eve_hal/Hdr/bt8xxemu.h" to "bt8xxemu.h"
cp "$INSTALL_PATH/common/eve_hal/Hdr/bt8xxemu.h" "bt8xxemu.h"
