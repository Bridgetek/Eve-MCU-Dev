# EVE-MCU-Dev GymInterval Arduino Example

[Back](../README.md)

The sketch needs additional files copied to setup it up in the Arduino IDE. Since the name of the sketch needs to match the directory name where the sketch is located the example sketch is kept in a special sketch directory named [`gyminterval_arduino`](gyminterval_arduino/).

## Sketch Directory

There are two methods of setting up the sketch directory. A python script is provided and manual instructions are also provided.

### Python Script

The python script `sketchbuild.py` can be used to copy the required files to the sketch automatically. It is run from the directory where the script is located and will find the API and example common directory. This script relies on the `sketchlib.py` module in the [`examples\arduino_sketches`](../../arduino_sketches/) directory.

The script will report the names of the files copied. There are no parameters for the script.

```
> python .\sketchbuild.py
Sketch name is gyminterval_arduino
..\common\eve_example.c -> gyminterval_arduino\eve_example.ino
..\common\eve_example.h -> gyminterval_arduino\eve_example.h
..\..\snippets\touch.c -> gyminterval_arduino\touch.ino
..\..\snippets\touch.h -> gyminterval_arduino\touch.h
..\..\snippets\maths\trig_furman.c -> gyminterval_arduino\trig_furman.ino
..\..\snippets\maths\trig_furman.h -> gyminterval_arduino\trig_furman.h
..\..\snippets\controls\arcs.c -> gyminterval_arduino\arcs.ino
..\..\snippets\controls\arcs.h -> gyminterval_arduino\arcs.h
..\..\snippets\controls\fonts.c -> gyminterval_arduino\fonts.ino
..\..\snippets\controls\fonts.h -> gyminterval_arduino\fonts.h
..\..\snippets\controls\sound.c -> gyminterval_arduino\sound.ino
..\..\snippets\controls\sound.h -> gyminterval_arduino\sound.h
..\..\..\include\EVE.h -> gyminterval_arduino\EVE.h
..\..\..\include\HAL.h -> gyminterval_arduino\HAL.h
..\..\..\include\MCU.h -> gyminterval_arduino\MCU.h
..\..\..\include\EVE_config.h -> gyminterval_arduino\EVE_config.h
..\..\..\include\EVE_defs.h -> gyminterval_arduino\EVE_defs.h
..\..\..\include\EVE_commands.h -> gyminterval_arduino\EVE_commands.h
..\..\..\include\EVE_debug.h -> gyminterval_arduino\EVE_debug.h
..\..\..\include\EVE_registers.h -> gyminterval_arduino\EVE_registers.h
..\..\..\source\EVE_API.c -> gyminterval_arduino\EVE_API.ino
..\..\..\source\EVE_HAL.c -> gyminterval_arduino\EVE_HAL.ino
..\..\..\ports\eve_arch_arduino\eve_arch_arduino.ino -> gyminterval_arduino\eve_arch_arduino.ino
..\..\..\ports\eve_arch_arduino\README.md -> gyminterval_arduino\README.md
..\..\..\source\extensions\bt82x_patch.c -> gyminterval_arduino\bt82x_patch.ino
..\..\..\source\extensions\bt82x_patch.h -> gyminterval_arduino\bt82x_patch.h
..\..\..\source\extensions\custom_touch_fw.h -> gyminterval_arduino\custom_touch_fw.h
..\..\..\source\extensions\custom_touch_fw.c -> gyminterval_arduino\custom_touch_fw.c
```

### Copy Files Manually

The following files **must** be copied into the `gyminterval_arduino` directory to be compiled into the sketch.

- From the `include` directory add the library header files:
  - EVE.h
  - MCU.h
  - HAL.h
  - EVE_commands.h
  - EVE_config.h
  - EVE_defs.h
  - EVE_registers.h
- From the `source` directory add the library files:
  - EVE_API.c
  - EVE_HAL.c
- From the `ports\eve_arch_arduino` directory add the MCU layer sketch for Arduino:
  - eve_arch_arduino.ino
  - README.md
- From the `examples\gyminterval\common` directory the files for the gyminterval example functionality:
  - eve_example.c
  - eve_example.h
- From the `examples\snippets` directory the files for the snippets functionality:
  - touch.c
  - touch.h
  - trig_furman.c
  - arcs.c
  - fonts.c
  - sound.c

- Extension source and header files are stored separately in the EVE-MCU-Dev
source tree:

  From `include/extensions`:

  - `custom_touch_fw.h` - EVE API 2, 3 and 4
  - `bt82x_patch.h` - EVE API 5

  From `source/extensions`:

  - `custom_touch_fw.c` - EVE API 2, 3 and 4
  - `bt82x_patch.c` - EVE API 5

When the Arduino library is generated, these files are copied into the
library root directory. The `extensions` directory structure is not retained

All files with a ".c" extension must be renamed to have the extension ".ino". In all the copied files, references to include files in the sketch must be changed from using angle brackets around the include file name to using quotes.

- `#include <EVE.h>` --> `#include "EVE.h"`
- `#include <HAL.h>` --> `#include "HAL.h"`
- `#include <MCU.h>` --> `#include "MCU.h"`
- `#include <EVE_commands.h>` --> `#include "EVE_commands.h"`
- `#include <EVE_config.h>` --> `#include "EVE_config.h"`
- `#include <EVE_defs.h>` --> `#include "EVE_defs.h"`
- `#include <EVE_debug.h>` --> `#include "EVE_debug.h"`
- `#include <EVE_registers.h>` --> `#include "EVE_registers.h"`
- `#include <extensions/bt82x_patch.h>` --> `#include "bt82x_patch.h"`
- `#include <extensions/custom_touch_fw.h>` --> `#include "custom_touch_fw.h"`

## Sketch Directory

The directory will look similar to this:

```
 Directory of <path>EVE-MCU-Dev\examples\gyminterval\arduino\gyminterval_arduino

EVE.h    
MCU.h      
HAL.h    
EVE_commands.h   
EVE_config.h 
EVE_defs.h 
EVE_debug.h 
EVE_registers.h   
EVE_API.ino   
EVE_HAL.ino   
bt82x_patch.ino      
bt82x_patch.h    
custom_touch_fw.ino      
custom_touch_fw.h 
eve_arch_arduino.ino  
README.md  
eve_example.ino       
eve_example.h         
gyminterval_arduino.ino
touch.ino
touch.h
trig_furman.ino
trig_furman.h
arcs.ino
arcs.h
fonts.ino
fonts.h
sound.ino
sound.h
```
The files above will appear as tabs in the sketch in the Arduino IDE.

## Compiling the GymInterval Arduino Example

The sketch can be verified and uploaded to the Arduino device. Please follow the instructions in the Arduino IDE and web pages on Arduino web site.

## Reusing the Example Code

This code can be reused by altering the code in the "example" files `eve_example.c`. If using the python script then the list of files for the example code will need updated.

## Updating the Configuration

If the sketch is to be changed to target a different display panel or EVE device then changes can be made to the file `EVE_config.h`. This is the main configuration file that is most often changed. If a display panel is changed then there are settings built-in to allow for several different popular screen types and sizes. Likewise, the EVE API is not compatible between generations of devices and the `FT8XX_TYPE` setting will have to change to any different device type.
