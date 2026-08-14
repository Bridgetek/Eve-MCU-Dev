# Typically imported from "sketchbuild.py" in an example directory.
import os
import re

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

# Copy and normalise file to be added to the sketch
def copy_norm(src_file, dest_file, flatten_filter):
    print(f"{src_file} -> {dest_file}")
    cppfile = []
    with open(src_file, "r") as fsrc:
        try:
            while line := fsrc.readline():
                cppadd = []
                line = line.rstrip()
                # Modify include statements to use local copy for sketch
                line = line.replace("<EVE.h>", "\"EVE.h\"")
                line = line.replace("<HAL.h>", "\"HAL.h\"")
                line = line.replace("<MCU.h>", "\"MCU.h\"")
                line = line.replace("<EVE_debug.h>", "\"EVE_debug.h\"")
                line = line.replace("<EVE_registers.h>", "\"EVE_registers.h\"")
                line = line.replace("<EVE_commands.h>", "\"EVE_commands.h\"")
                line = line.replace("<EVE_config.h>", "\"EVE_config.h\"")
                line = line.replace("<patch_base.h>", "\"patch_base.h\"")
                line = line.replace("<custom_touch.h>", "\"custom_touch.h\"")
                # Remove directory paths in the files that need flattened for the sketch
                for fl in flatten_filter:
                    line = line.replace(f"\"{fl}/", "\"")
                # Global static consts moved into PROGMEM storage on Arduino
                line = re.sub(r"^static const uint8_t ", "constexpr PROGMEM static const uint8_t ", line)
                line = re.sub(r'^const uint8_t\s*(\w+)\s*\[', r'PROGMEM const uint8_t \g<1> [', line)
                # Add PROGMEM storage linkage for eve_example.h
                if dest_file.endswith("eve_example.h"):
                    if line == "#include <stdint.h>":
                        cppadd = [
                                "#include <string.h>",
                                "",
                                "#if defined(ESP8266) || defined(ESP32)",
                                "#include <pgmspace.h>",
                                "#elif defined(__AVR__)",
                                "#include <avr/pgmspace.h>",
                                "#else",
                                "#define PROGMEM",
                                "#define memcpy_P(A,B,C) memcpy((A),(B),(C))",
                                "#define pgm_read_byte(addr) (*(const unsigned char *)(addr))",
                                "#endif",
                                "",
                        ]
                        print("eve_example.h updated for PROGMEM")
                # Add PROGMEM storage read for eve_images.ino
                elif dest_file.endswith("eve_images.ino"):
                    match = re.match(r"^(\s*)(\w*\[i\]) = \*(img\+\+);", line)
                    if match:
                        line = None
                        cppadd = [
                            f"{match.group(1)}{match.group(2)} = pgm_read_byte({match.group(3)});",
                        ]
                        print("eve_images.ino updated for accessing PROGMEM")
                # Add PROGMEM storage read for eve_fonts.ino
                elif dest_file.endswith("eve_fonts.ino"):
                    # EVE_LIB_WriteDataToRAMG(font0, font0_size, font0_offset);
                    match1 = re.match(r"^(\s*)EVE_LIB_WriteDataToRAMG\((\w+),\s(\w+),\s(\w+)\);", line)
                    # const EVE_GPU_FONT_HEADER *font0_hdr = (const EVE_GPU_FONT_HEADER *)font0;
                    match2 = re.match(r"^const EVE_GPU_FONT_HEADER \*(\w+)\s=\s\(const EVE_GPU_FONT_HEADER \*\)(\w+);", line)
                    if match1:
                        line = None
                        cppadd = [
                                f"{match1.group(1)}memcpy_P(&font0_header, {match1.group(2)}, sizeof(font0_header));",
                                f"{match1.group(1)}/* Read the data from the program memory into RAM. */",
                                f"{match1.group(1)}uint8_t pgm[16];",
                                f"{match1.group(1)}uint32_t pgmoffset;",
                                f"{match1.group(1)}for (pgmoffset = 0; pgmoffset < {match1.group(3)}; pgmoffset+=16)",
                                f"{match1.group(1)}{{",
                                f"{match1.group(1)}    // Maximum of pgm buffer",
                                f"{match1.group(1)}    uint32_t chunk = sizeof(pgm);",
                                f"{match1.group(1)}    if (pgmoffset + chunk > {match1.group(3)})",
                                f"{match1.group(1)}    {{",
                                f"{match1.group(1)}        chunk = {match1.group(3)} - pgmoffset;",
                                f"{match1.group(1)}    }}",
                                f"{match1.group(1)}    // Load the pgm buffer",
                                f"{match1.group(1)}    memcpy_P(pgm, &{match1.group(2)}[pgmoffset], chunk);",
                                f"{match1.group(1)}    EVE_LIB_WriteDataToRAMG(pgm, chunk, {match1.group(4)} + pgmoffset);",
                                f"{match1.group(1)}}}",
                        ]
                        print("eve_fonts.ino updated for accessing PROGMEM")
                    if match2:
                        line = None
                        cppadd = [
                                f"EVE_GPU_FONT_HEADER font0_header;",
                                f"const EVE_GPU_FONT_HEADER *{match2.group(1)} = &font0_header;",
                        ]
                        print("eve_fonts.ino updated for PROGMEM compatible globals")
                # Add PROGMEM storage read for patch_base.ino
                elif dest_file.endswith("patch_base.ino"):
                    match = re.match(r"^(\s*)EVE_LIB_WriteDataToCMD\((\w+),\s(\w+)\);", line)
                    if match:
                        len = int(match.group(3))
                        line = None
                        cppadd = [
                                f"{match.group(1)}/* Read the data from the program memory into CMD. */",
                                f"{match.group(1)}uint8_t pgm[16];",
                                f"{match.group(1)}uint32_t pgmoffset, pgmchunk;",
                                f"{match.group(1)}for (pgmoffset = 0; pgmoffset < {len}; pgmoffset += 16)",
                                f"{match.group(1)}{{",
                                f"{match.group(1)}    // Maximum of pgm buffer",
                                f"{match.group(1)}    uint32_t chunk = sizeof(pgm);",
                                f"{match.group(1)}    if (pgmoffset + chunk > {len})",
                                f"{match.group(1)}    {{",
                                f"{match.group(1)}        chunk = {len} - pgmoffset;",
                                f"{match.group(1)}    }}",
                                f"{match.group(1)}    // Load the pgm buffer",
                                f"{match.group(1)}    memcpy_P(pgm, &{match.group(2)}[pgmoffset], chunk);",
                                f"{match.group(1)}    EVE_LIB_WriteDataToCMD(pgm, chunk);",
                                f"{match.group(1)}}}",
                        ]
                        print("patch_base.ino updated for accessing PROGMEM")
                # Add PROGMEM storage read for EVE_API.ino (CUSTOM_TOUCH)
                elif dest_file.endswith("EVE_API.ino"):
                    match = re.match(r"^(\s*)EVE_LIB_WriteDataToCMD\((\w+),\s*sizeof\(([^)]+)\)\);", line)
                    if match:
                        line = None
                        cppadd = [
                                f"{match.group(1)}/* Read the data from the program memory into CMD. */",
                                f"{match.group(1)}uint8_t pgm[16];",
                                f"{match.group(1)}uint32_t pgmoffset, pgmchunk;",
                                f"{match.group(1)}for (pgmoffset = 0; pgmoffset < sizeof({match.group(2)}); pgmoffset += 16)",
                                f"{match.group(1)}{{",
                                f"{match.group(1)}    // Maximum of pgm buffer",
                                f"{match.group(1)}    uint32_t chunk = sizeof(pgm);",
                                f"{match.group(1)}    if (pgmoffset + chunk > sizeof({match.group(2)}))",
                                f"{match.group(1)}    {{",
                                f"{match.group(1)}        chunk = sizeof({match.group(2)}) - pgmoffset;",
                                f"{match.group(1)}    }}",
                                f"{match.group(1)}    // Load the pgm buffer",
                                f"{match.group(1)}    memcpy_P(pgm, &{match.group(2)}[pgmoffset], chunk);",
                                f"{match.group(1)}    EVE_LIB_WriteDataToCMD(pgm, chunk);",
                                f"{match.group(1)}}}",
                        ]
                        print("EVE_API.ino updated for accessing PROGMEM")

                if line != None:
                    cppfile.append(line)

                for a in cppadd:
                    cppfile.append(a)

            with open(dest_file, "w") as file:
                for line in cppfile:
                    file.write(line + "\n")

        except Exception as inst:
            print("Error: default file handling -", inst)
            # File is binary or could not be parsed
            with open(src_file, "rb") as file:
                bdata = file.read()
            with open(dest_file, "wb") as file:
                file.write(bdata)