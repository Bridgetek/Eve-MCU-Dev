# Typically run with "python libbuild.py --dist ../EVE-MCU-Dev --api 5"
#
# This requires a C preprocessor callable with the command line "cpp". 
#
import subprocess
import shutil
import re
import os
import argparse

eve_api = 5
eve_sub_api = 0

# Parse arguments
parser = argparse.ArgumentParser(description="Library Builder for EVE")
parser.add_argument("--src", required=True, help="distribution directory for EVE-MCU-Dev")
parser.add_argument("--dest", help="destination directory for Arduino library (default is BtEve<API>)")
parser.add_argument("--api", default=eve_api, help="EVE API to build library for (valid values are 1 to 5)")
parser.add_argument("--apisub", default=eve_sub_api, help="EVE SUB API to build library for (for EVE API 2 must be 1 or 2)")
(args, rem) = parser.parse_known_args()
if (args.api): eve_api = int(args.api)
if (args.apisub): eve_sub_api = int(args.apisub)

# Check valid input
if (eve_api < 1) or (eve_api > 5):
    raise Exception("Invalid EVE API")
if (eve_api == 2) and ((eve_sub_api < 1) or (eve_sub_api > 2)):
    raise Exception("Invalid EVE SUB API for EVE API 2")

# Check for a suitable preprocessor
try:
    cppcmd = ['cpp', '--version']
    cpptest = subprocess.run(cppcmd, stdout=subprocess.PIPE)
except:
    raise Exception("Requires a GNU C preprocessor (cpp) on the system PATH.")

print(f"Making EVE API {eve_api}")
if eve_sub_api > 0: 
    print(f"(EVE SUB VER {eve_sub_api})")

# Generate name of library
if eve_sub_api > 1:
    library_name = f"BtEve{eve_api}_{eve_sub_api}"
else:
    library_name = f"BtEve{eve_api}"
print(f"Library name is {library_name}")

src_api = os.path.normpath(args.src)
if not os.path.exists(src_api):
    raise Exception("The distribution directory doesn't exist")
if args.dest:
    dest_lib = os.path.normpath(args.dest)
else:
    dest_lib = os.path.normpath(library_name)

def template(file_in, file_out, lib, api, subapi, apidefs):
    cppfile = []
    flag = True
    apiver = f"{api}"
    if subapi > 1: 
        apisubver = f"{subapi}"
        ver = apiver + "_" + apisubver
    else: 
        if subapi > 0: 
            apisubver = f"{subapi}"
        else:
            apisubver = ""
        ver = apiver
    if api == 1:
        apidev = "FT800/FT801"
    elif api == 2:
        if subapi != 2:
            apidev = "FT810/FT811/FT812/FT813"
        else:
            apidev = "BT880/BT881/BT882/BT883"
    elif api == 3:
        apidev = "BT815/BT816"
    elif api == 4:
        apidev = "BT817/BT818"
    elif api == 5:
        apidev = "BT820"
    with open(file_in, "r") as file:
        while line := file.readline():
            if re.findall(r"/\*END API\*/", line): 
                flag = True
            line = re.sub(r"/\*EVE API VER\*/", ver, line)
            line = re.sub(r"/\*EVE API\*/", apiver, line)
            line = re.sub(r"/\*EVE SUB API\*/", apisubver, line)
            line = re.sub(r"/\*EVE DEV\*/", apidev, line)
            line = re.sub(r"/\*EVE LIB\*/", lib, line)
            if flag: 
                cppfile.append(line.rstrip())
            if re.findall(r"/\*BEGIN API\*/", line):
                flag = False
                cppfile.extend(apidefs)

    with open(file_out, "w") as file:
        for line in cppfile:
            file.write(line + "\n")

# Collate header files needed (from include directory)
dist_inc_files = ["EVE.h", "HAL.h", "MCU.h", "FT8xx.h"]
if eve_api == 1:
    dist_inc_files.append("FT80x.h")
elif eve_api == 2:
    dist_inc_files.append("FT81x.h")
elif eve_api == 3:
    dist_inc_files.append("BT81x.h")
elif eve_api == 4:
    dist_inc_files.append("BT81x.h")
elif eve_api == 5:
    dist_inc_files.append("BT82x.h")

# Destination API file
dest_api = os.path.join(dest_lib,"EVE_API.c")

# Collate source files needed
dist_source_files = []
dist_source_files.append((os.path.join(src_api,"source","EVE_API.c"), dest_api))
dist_source_files.append((os.path.join(src_api,"source","EVE_HAL.c"), os.path.join(dest_lib,"EVE_HAL.c")))
dist_source_files.append((os.path.join(src_api,"ports","eve_arch_arduino","eve_arch_arduino.ino"), os.path.join(dest_lib,"EVE_MCU.cpp")))
if eve_api == 5:
    dist_source_files.append((os.path.join(src_api,"ports","eve_bt82x","patch_base.c"), os.path.join(dest_lib,"patch_base.c")))
    dist_source_files.append((os.path.join(src_api,"ports","eve_bt82x","patch_base.h"), os.path.join(dest_lib,"patch_base.h")))
for d in dist_inc_files:
    dist_source_files.append((os.path.join(src_api,"include",d), os.path.join(dest_lib,d)))

# Copy API source and header files
try:
    os.makedirs(dest_lib, exist_ok=True)
    for d in dist_source_files:
        srcf, destf = d
        print(f"{srcf} -> {destf}")
        shutil.copyfile(srcf, destf)
except:
    raise Exception("The distribution directory doesn't look like EVE-MCU-Dev")

# Make the test directory
test_dir = os.path.join(dest_lib, "test")
os.makedirs(test_dir, exist_ok=True)

# Template header files (Template files pass 1)
template_files = []
# EVE_config.h file
template_files.append(("EVE_config.h.template", os.path.join(dest_lib,"EVE_config.h")))
# library.properties file
template_files.append(("library.properties.template", os.path.join(dest_lib,"library.properties")))
# README.md file
template_files.append(("README.md.template", os.path.join(dest_lib,"README.md")))
# README.md file
template_files.append(("test.ino.template", os.path.join(test_dir,"test.ino")))
for t in template_files:
    srcf, destf = t
    print(f"{srcf} -> {destf}")
    template(srcf, destf, library_name, eve_api, eve_sub_api, "")

# Command line for preprocessor
cppcmd = ['cpp', f'-DEVE_API={eve_api}']
if eve_sub_api > 1:
    cppcmd.append(f'-DEVE_SUB_API={eve_sub_api}')

# Command line to preprocess the library files
cppcmd.append(dest_api)
coderes = subprocess.run(cppcmd, stdout=subprocess.PIPE)

# Command line to harvest all #defines from the source code
cppcmd.insert(1, '-dM')
defineres = subprocess.run(cppcmd, stdout=subprocess.PIPE)

if (coderes.returncode == 0) and (defineres.returncode == 0):
    cppoutput = coderes.stdout.decode('utf-8')
    cpplines = cppoutput.splitlines()

    # Find function declarations in the header file
    cppre = re.compile(r'^(void|int.*|uint.*|char)\s(EVE_\w*|eve_\w*)\((.*)\)[;]')
    cfndefs = []
    for line in cpplines:
        if cppre.search(line):
            cfndefs.append(line)
    cfndefs = list(dict.fromkeys(cfndefs))

    cppdefs = []
    for line in cfndefs:
        cdefl = cppre.split(line)
        # cdefl[0] = ""
        # cdefl[1] = "void|int.*|uint.*|char"
        # cdefl[2] = "EVE_\w*"
        # cdefl[3] = "", "void", "uint32_t options", or "..."
        # cdefl[4] = ""
        cdefl[0] = re.sub("EVE_", "", cdefl[2])
        capiparams = cdefl[3].split(",")
        cparam = []
        cppparam = []
        variac = False
        variacfmt = ""
        for i, p in enumerate(capiparams):
            if p.strip() == "...": 
                variac = True
            px = p.strip().split()
            if (len(px) > 1):
                cppparam.append(p)
                cparam.append(p.split()[-1].strip("*"))
                if variac == False:
                    variacfmt = p.split()[-1].strip("*")
            else:
                if variac:
                    cppparam.append("...")
                    cparam.append("myargs")
                else:
                    cppparam.append("void")
                    cparam.append("")
        cparamtext = ""
        cppparamtext = ""
        for i,cp in enumerate(cparam):
            cparamtext += cp
            if i < len(cparam) - 1:
                cparamtext += ", "
        for i,cp in enumerate(cppparam):
            cppparamtext += cp
            if i < len(cppparam) - 1:
                cppparamtext += ", "

        addnret = ""
        addnvariac = ""
        addevariac = ""
        if variac:
            addnvariac = f"va_list myargs; va_start(myargs, {variacfmt}); "
            addevariac = "va_end(myargs); "
        elif cdefl[0].startswith("LIB_") and not cdefl[1].startswith("void"):
            addnret = "return "
        cppline = f"    {cdefl[1]} {cdefl[0]}({cppparamtext}) {{ {addnvariac}{addnret}::{cdefl[2]}({cparamtext}); {addevariac}}};"
        cppdefs.append(cppline)

    cppdefouput = defineres.stdout.decode('utf-8')
    definelines = cppdefouput.splitlines()

    defre = re.compile(r'^\#define\s(EVE_\w*)\s*(.+)')
    cconstdefs = []
    for line in definelines:
        if defre.search(line):
            definep =line.split()
            if len(definep) > 2:
                cppline = None
                definep[0] = re.sub("EVE_", "", definep[1])
                if line.endswith("}"):
                    # deal with arrays
                    cppline = f"    /*array for {definep[0]} */"
                elif definep[0].startswith("ENC_"):
                    pass
                else:
                    cppline = f"    const uint32_t {definep[0]} = {definep[2]};"
                if cppline:
                    cppdefs.append(cppline)
    #cconstdefs = list(dict.fromkeys(cconstdefs))

# Library main class files (template files pass 2)
template_files = []
template_files.append(("bteve.cpp.template", os.path.join(dest_lib,f"{library_name}.cpp")))
template_files.append(("bteve.h.template", os.path.join(dest_lib,f"{library_name}.h")))

for t in template_files:
    srcf, destf = t
    print(f"{srcf} -> {destf}")
    template(srcf, destf, library_name, eve_api, eve_sub_api, cppdefs)

# Make the examples directory
examples_dir = os.path.join(dest_lib, "examples")
os.makedirs(examples_dir, exist_ok=True)

# Copy all the examples
print(f"examples -> {examples_dir}")
shutil.copytree(os.path.normpath("examples"), examples_dir, dirs_exist_ok=True)
