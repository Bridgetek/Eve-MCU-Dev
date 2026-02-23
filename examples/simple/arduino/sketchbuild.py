# Typically run with "python sketchbuild.py"
# This will find the first subdirectory name ending in _arduino which it will use as the sketch name
import shutil
import os
import re

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
    print(f"{src_file} -> {dest_file}")
    with open(src_file, "r") as fsrc:
        with open(dest_file, "w") as fdest:
            while strsrc := fsrc.readline():
                # Modify include statements to use local copy for sketch
                strsrc = strsrc.replace("<EVE.h>", "\"EVE.h\"")
                strsrc = strsrc.replace("<HAL.h>", "\"HAL.h\"")
                strsrc = strsrc.replace("<MCU.h>", "\"MCU.h\"")
                strsrc = strsrc.replace("<FT8xx.h>", "\"FT8xx.h\"")
                strsrc = strsrc.replace("<EVE_config.h>", "\"EVE_config.h\"")
                strsrc = strsrc.replace("<patch_base.h>", "\"patch_base.h\"")
                # Global static consts moved into PROGMEM storage on Arduino
                strsrc = re.sub(r"^static const uint8_t ", "constexpr PROGMEM static const uint8_t ", strsrc)
                # Add PROGMEM storage linkage for eve_example.h
                if dest_file.endswith("eve_example.h"):
                    if strsrc == "#include <stdint.h>\n":
                        fdest.write(strsrc) 
                        strsrc = "#include <avr/pgmspace.h>\n"
                        print("eve_example.h updated for PROGMEM")
                # Add PROGMEM storage read for eve_images.ino
                if dest_file.endswith("eve_images.ino"):
                    match = re.match(r"^(\s*)(\w*\[i\]) = \*(img\+\+);", strsrc)
                    if match:
                        strsrc = f"{match.group(1)}{match.group(2)} = pgm_read_byte({match.group(3)});\n"
                        print("eve_images.ino updated for accessing PROGMEM")
                # Add PROGMEM storage read for eve_fonts.ino
                if dest_file.endswith("eve_fonts.ino"):
                    # EVE_LIB_WriteDataToRAMG(font0, font0_size, font0_offset);
                    match = re.match(r"^(\s*)EVE_LIB_WriteDataToRAMG\((\w+),\s(\w+),\s(\w+)\);", strsrc)
                    if match:
                        fdest.writelines(
                            (
                                f"{match.group(1)}memcpy_P(&font0_header, {match.group(2)}, sizeof(font0_header));\n",
                                "\n",
                                f"{match.group(1)}/* Read the data from the program memory into RAM. */\n",
                                f"{match.group(1)}uint8_t pgm[16];\n",
                                f"{match.group(1)}uint32_t pgmoffset, pgmchunk;\n",
                                f"{match.group(1)}for (pgmoffset = 0; pgmoffset < {match.group(3)}; pgmoffset+=16)\n",
                                f"{match.group(1)}{{\n",
                                f"{match.group(1)}    // Maximum of pgm buffer\n",
                                f"{match.group(1)}    uint32_t chunk = sizeof(pgm);\n",
                                f"{match.group(1)}    if (pgmoffset + chunk > {match.group(3)})\n",
                                f"{match.group(1)}    {{\n",
                                f"{match.group(1)}        chunk = {match.group(3)} - pgmoffset;\n",
                                f"{match.group(1)}    }}\n",
                                f"{match.group(1)}    // Load the pgm buffer\n",
                                f"{match.group(1)}    memcpy_P(pgm, &{match.group(2)}[pgmoffset], chunk);\n",
                                f"{match.group(1)}    EVE_LIB_WriteDataToRAMG(pgm, chunk, {match.group(4)} + pgmoffset);\n",
                                f"{match.group(1)}}}\n",
                            )
                        )
                        strsrc = ""
                        print("eve_fonts.ino updated for accessing PROGMEM")
                    # const EVE_GPU_FONT_HEADER *font0_hdr = (const EVE_GPU_FONT_HEADER *)font0;
                    match = re.match(r"^const EVE_GPU_FONT_HEADER \*(\w+)\s=\s\(const EVE_GPU_FONT_HEADER \*\)(\w+);", strsrc)
                    if match:
                        fdest.writelines(
                            (
                                f"EVE_GPU_FONT_HEADER font0_header;\n",
                                f"const EVE_GPU_FONT_HEADER *{match.group(1)} = &font0_header;\n",
                            )
                        )
                        strsrc = ""
                        print("eve_fonts.ino updated for PROGMEM compatible globals")
                # Add PROGMEM storage read for patch_base.ino
                if dest_file.endswith("patch_base.ino"):
                    match = re.match(r"^(\s*)EVE_LIB_WriteDataToCMD\((\w+),\s(\w+)\);", strsrc)
                    if match:
                        len = int(match.group(3))
                        fdest.writelines(
                            (
                                f"{match.group(1)}/* Read the data from the program memory into CMD. */\n",
                                f"{match.group(1)}uint8_t pgm[16];\n",
                                f"{match.group(1)}uint32_t pgmoffset, pgmchunk;\n",
                                f"{match.group(1)}for (pgmoffset = 0; pgmoffset < {len}; pgmoffset += 16)\n",
                                f"{match.group(1)}{{\n",
                                f"{match.group(1)}    // Maximum of pgm buffer\n",
                                f"{match.group(1)}    uint32_t chunk = sizeof(pgm);\n",
                                f"{match.group(1)}    if (pgmoffset + chunk > {len})\n",
                                f"{match.group(1)}    {{\n",
                                f"{match.group(1)}        chunk = {len} - pgmoffset;\n",
                                f"{match.group(1)}    }}\n",
                                f"{match.group(1)}    // Load the pgm buffer\n",
                                f"{match.group(1)}    memcpy_P(pgm, &{match.group(2)}[pgmoffset], chunk);\n",
                                f"{match.group(1)}    EVE_LIB_WriteDataToCMD(pgm, chunk);\n",
                                f"{match.group(1)}}}\n",
                            )
                        )
                        strsrc = ""
                        print("patch_base.ino updated for accessing PROGMEM")

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
