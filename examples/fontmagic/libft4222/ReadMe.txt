Windows
=======

Important Information about Windows builds using Visual Studio and Visual Studio Code.

To compile this you will have to download the libFT4222 library for Windows. Recommended version is v1.4.7 or later. It is available from the FTDI website:

https://ftdichip.com/software-examples/ft4222h-software-examples/

Download the LibFT4222-vx.x.x.zip file to this directory and unzip it.

The following files are used in this sample code.

- imports\LibFT4222\dll/amd64\LibFT4222-64.lib
- imports\LibFT4222\dll/amd64\LibFT4222-64.dll
- imports\ftd2xx\dll\amd64\ftd2xx.lib 
- imports\ftd2xx\ftd2xx.h
- imports\ftd2xx\WinTypes.h
- imports\LibFT4222\inc\LibFT4222\LibFT4222.h

The example application will load and use the LibFT4222-64.dll file. This file must be available either locally (in the same directory as the example application) or on the system path (recommended "C:\Windows\System32").

Visual Studio Code
------------------

To use this example with Microsoft Visual Studio Code follow the instructions in this web page.

https://code.visualstudio.com/docs/cpp/config-msvc

The configuration, build and launch files are found in the ".vscode" folder. 

Open Visual Studio Code using the command "code ." from a "Developer Command Prompt for VS <version>" to ensure that the build environment is setup correctly.

When the example application is launched in Code it will add the location of the libmpsse.dll file to the PATH environment variable of the running process.

Program executables and object files are placed into the ".vscode\build" folder. 

Visual Studio
-------------

The Visual Studio project files for the example are found in the "VisualStudio" folder. Program executables and object files are placed into the "VisualStudio\Debug" folder. 

When the example application is launched in Visual Studio it will add the location of the libmpsse.dll file to the PATH environment variable of the running process.

Linux
=====

The "Makefile" will compile the required files for the example application. The LibFT4222 library for Linux will be taken from the "/usr/local/lib" directory and the include files from "/usr/local/include".
