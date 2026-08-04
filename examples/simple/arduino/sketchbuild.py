# Typically run with "python sketchbuild.py"
# This will find the first subdirectory name ending in _arduino which it will use as the sketch name
import os
import sys
sys.path.insert(0, "../../arduino_sketches")

from sketchlib import *

sketch = None
for x in os.scandir('.'):
    if (x.is_dir()):
        if x.name.endswith("_arduino"):
            sketch = x.name

if sketch == None:
    raise Exception("No subdirectory containing a sketch")

print(f"Sketch name is {sketch}")

# Collate source files needed
dist_source_files = []

# Source directories for source files to be copied to sketch
src_example = os.path.normpath("../common")
src_snippets = os.path.normpath("../../snippets")
src_api_source = os.path.normpath("../../../source")
src_api_include = os.path.normpath("../../../include")
src_port = os.path.normpath("../../../ports/eve_arch_arduino")
src_patch = os.path.normpath("../../../ports/eve_bt82x")

src_flatten_dirs = []

# Collate files needed for sketch
dist_source_files.extend(add_files(src_example, sketch, ["eve_example.c", "eve_example.h", "eve_fonts.c", "eve_images.c"]))
dist_source_files.extend(add_files(src_snippets, sketch, ["touch.c", "touch.h"]))
dist_source_files.extend(add_files(src_api_include, sketch, ["EVE.h", "HAL.h", "MCU.h", "EVE_debug.h", "EVE_registers.h", "EVE_commands.h", "EVE_config.h"]))
dist_source_files.extend(add_files(src_api_source, sketch, ["EVE_API.c", "EVE_HAL.c"]))
dist_source_files.extend(add_files(src_port, sketch, ["eve_arch_arduino.ino", "README.md"]))
dist_source_files.extend(add_files(src_patch, sketch, ["patch_base.c", "patch_base.h"]))

# Copy files into sketch
try:
    for d in dist_source_files:
        srcf, destf = d
        copy_norm(srcf, destf, src_flatten_dirs)
except:
    raise Exception("The distribution directory doesn't look like EVE-MCU-Dev")
