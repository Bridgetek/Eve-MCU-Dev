# Typically run with "python sketchbuild.py"
# This will find the first subdirectory name ending in _arduino which it will use as the sketch name
import shutil
import os

sketch = None
for x in os.scandir('.'):
    if (x.is_dir()):
        if x.name.endswith("_arduino"):
            sketch = x.name

if sketch == None:
    raise Exception("No subdirectory containing a sketch")

print(f"Sketch name is {sketch}")

# Add files to list of files to copy
def add_files(src_dir, dest_dir, file_list):
    added_files = []
    if not os.path.exists(src_dir):
        raise Exception(f"The directory \"{src_dir}\" doesn't exist")
    # Add files to the list
    try:
        for d in file_list:
            dino = d
            if os.path.splitext(d)[1] == '.c':
                dino = os.path.splitext(d)[0] + '.ino'
            added_files.append((os.path.join(src_dir,d), os.path.join(dest_dir,dino)))
    except:
        raise Exception("The directory \"{src_dir}\" doesn't look correct")
    return added_files

def copy_norm(src_file, dest_file):
    print(f"{srcf} -> {destf}")
    with open(srcf, "r") as fsrc:
        with open(destf, "w") as fdest:
            strsrc = fsrc.read()
            # Modify include statements to use local copy for sketch
            strsrc = strsrc.replace("<EVE.h>", "\"EVE.h\"")
            strsrc = strsrc.replace("<HAL.h>", "\"HAL.h\"")
            strsrc = strsrc.replace("<MCU.h>", "\"MCU.h\"")
            strsrc = strsrc.replace("<FT8xx.h>", "\"FT8xx.h\"")
            strsrc = strsrc.replace("<EVE_config.h>", "\"EVE_config.h\"")
            strsrc = strsrc.replace("<patch_base.h>", "\"patch_base.h\"")
            fdest.write(strsrc) 

# Collate source files needed
dist_source_files = []

# Source directories for source files to be copied to sketch
src_example = os.path.normpath("../common")
src_snippets = os.path.normpath("../../snippets")
src_api_source = os.path.normpath("../../../source")
src_api_include = os.path.normpath("../../../include")
src_port = os.path.normpath("../../../ports/eve_arch_arduino")
src_patch = os.path.normpath("../../../ports/eve_bt82x")

# Collate files needed for sketch
dist_source_files.extend(add_files(src_example, sketch, ["eve_example.c", "eve_example.h", "eve_fonts.c", "eve_images.c"]))
dist_source_files.extend(add_files(src_snippets, sketch, ["touch.c", "touch.h"]))
dist_source_files.extend(add_files(src_api_include, sketch, ["EVE.h", "HAL.h", "MCU.h", "FT8xx.h", "EVE_config.h",
    "FT80x.h", "FT81x.h", "BT81x.h", "BT81x.h", "BT82x.h"]))
dist_source_files.extend(add_files(src_api_source, sketch, ["EVE_API.c", "EVE_HAL.c"]))
dist_source_files.extend(add_files(src_port, sketch, ["eve_arch_arduino.ino", "README.md"]))
dist_source_files.extend(add_files(src_patch, sketch, ["patch_base.c", "patch_base.h"]))

# Copy files into sketch
try:
    for d in dist_source_files:
        srcf, destf = d
        copy_norm(srcf, destf)
except:
    raise Exception("The distribution directory doesn't look like EVE-MCU-Dev")
