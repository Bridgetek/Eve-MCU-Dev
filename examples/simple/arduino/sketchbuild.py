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

src_example = os.path.normpath("../common")
if not os.path.exists(src_example):
    raise Exception("The example directory doesn't exist")

# Collate example files needed (from example common directory)
example_common_files = ["eve_example.c", "eve_example.h", "eve_calibrate.c", "eve_fonts.c", "eve_helper.c", "eve_images.c"]

# Copy example source and header files
try:
    for d in example_common_files:
        srcf, destf = (os.path.join(src_example,d), os.path.join(sketch,d))
        print(f"{srcf} -> {destf}")
        shutil.copyfile(srcf, destf)
except:
    raise Exception("The example common directory doesn't look correct")

# Get the API directory
src_api = os.path.normpath("../../../")
if not os.path.exists(src_api):
    raise Exception("The distribution directory doesn't exist")

# Collate header files needed (from include directory)
dist_inc_files = ["EVE.h", "HAL.h", "MCU.h", "FT8xx.h", "EVE_config.h",
    "FT80x.h", "FT81x.h", "BT81x.h", "BT81x.h", "BT82x.h"]

# Collate source files needed
dist_source_files = []
dist_source_files.append((os.path.join(src_api,"source","EVE_API.c"), os.path.join(sketch,"EVE_API.c")))
dist_source_files.append((os.path.join(src_api,"source","EVE_HAL.c"), os.path.join(sketch,"EVE_HAL.c")))
dist_source_files.append((os.path.join(src_api,"ports","eve_arch_arduino","eve_arch_arduino.ino"), os.path.join(sketch,"eve_arch_arduino.ino")))
dist_source_files.append((os.path.join(src_api,"ports","eve_arch_arduino","README.md"), os.path.join(sketch,"README.md")))
dist_source_files.append((os.path.join(src_api,"ports","eve_bt82x","patch_base.c"), os.path.join(sketch,"patch_base.c")))
dist_source_files.append((os.path.join(src_api,"ports","eve_bt82x","patch_base.h"), os.path.join(sketch,"patch_base.h")))
for d in dist_inc_files:
    dist_source_files.append((os.path.join(src_api,"include",d), os.path.join(sketch,d)))

# Copy API source and header files
try:
    for d in dist_source_files:
        srcf, destf = d
        print(f"{srcf} -> {destf}")
        shutil.copyfile(srcf, destf)
except:
    raise Exception("The distribution directory doesn't look like EVE-MCU-Dev")
