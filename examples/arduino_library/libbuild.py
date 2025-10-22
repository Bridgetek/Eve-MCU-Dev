# Typically run with "python libbuild.py --dist ../EVE-MCU-Dev --api 5"
#
# This requires a C preprocessor callable with the command line "cpp". 
#
import subprocess
import re
import os
import argparse

# Default API version
eve_api = 5
eve_sub_api = 0

# Default path to API root
src_api = os.path.normpath(os.path.join("..",".."))

parser = argparse.ArgumentParser(description="Library Builder for EVE")
parser.add_argument("--src", default=src_api, help="distribution directory for EVE-MCU-Dev")
parser.add_argument("--dest", help="destination directory for Arduino library (default is BtEve<API>)")
parser.add_argument("--api", default=eve_api, help="EVE API to build library for (valid values are 1 to 5)")
parser.add_argument("--apisub", default=eve_sub_api, help="EVE SUB API to build library for (for EVE API 2 must be 1 or 2)")
(args, rem) = parser.parse_known_args()
if (args.api): eve_api = int(args.api)
if (args.apisub): eve_sub_api = int(args.apisub)

# Check valid input
if (eve_api < 1) or (eve_api > 5):
    raise Exception("Invalid value for --api")
if (eve_api == 2) and ((eve_sub_api < 1) or (eve_sub_api > 2)):
    raise Exception("Invalid value for --apisub required when --api is 2")

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
    str_lib_name = f"Bridgetek_EVE{eve_api}_{eve_sub_api}"
else:
    str_lib_name = f"Bridgetek_EVE{eve_api}"
print(f"Library name is {str_lib_name}")

str_api_version = f"{eve_api}"
if eve_sub_api > 1: 
    str_api_sub_version = f"{eve_sub_api}"
    str_full_version = str_api_version + "_" + str_api_sub_version
else: 
    if eve_sub_api > 0: 
        str_api_sub_version = f"{eve_sub_api}"
    else:
        str_api_sub_version = ""
    str_full_version = str_api_version

src_api = os.path.normpath(args.src)
if not os.path.exists(src_api):
    raise Exception("The distribution directory doesn't exist")
if args.dest:
    dest_lib = os.path.normpath(args.dest)
else:
    dest_lib = os.path.normpath(str_lib_name)

# Check API source and header files exist
if not (os.path.exists(os.path.join(src_api, "source")) and 
        os.path.exists(os.path.join(src_api, "include")) and
        os.path.exists(os.path.join(src_api, "ports"))):
    raise Exception("The distribution directory doesn't look like EVE-MCU-Dev")

# Function to turn template files into final versions
def template(file_in, file_out, cpplib, api, subapi, str_full_version, str_api_version, apidefs, apiproto, apiconst):
    cppfile = []
    flag = 0
        
    # defaults for each generation
    if api == 1:
        apidev = "FT800/FT801"
        apilib = "FT800 and FT801"
        defres = "WQVGA" # 480 x 272
    elif api == 2:
        if subapi != 2:
            apidev = "FT810/FT811/FT812/FT813"
            apilib = "FT810 FT811 FT812 and FT813"
            defres = "WVGA" # 800 x 480
        else:
            apidev = "BT880/BT881/BT882/BT883"
            apilib = "BT880 BT881 BT882 and BT883"
            defres = "WQVGA" # 480 x 272
    elif api == 3:
        apidev = "BT815/BT816"
        apilib = "BT815 and BT816"
        defres = "WVGA" # 800 x 480
    elif api == 4:
        apidev = "BT817/BT818"
        apilib = "BT817 and BT818"
        defres = "WSVGA" # 1024 x 600
    elif api == 5:
        apidev = "BT820"
        apilib = "BT820"
        defres = "WUXGA" # 1920 x 1200

    # Replace markers with values
    with open(file_in, "r") as file:
        try:
            while line := file.readline():
                line = re.sub(r"### EVE API VER ###", str_full_version, line)
                line = re.sub(r"### EVE API ###", str_api_version, line)
                line = re.sub(r"### EVE SUB API ###", str_api_sub_version, line)
                line = re.sub(r"### EVE DEV ###", apidev, line)
                line = re.sub(r"### EVE LIB NAME ###", apilib, line)
                line = re.sub(r"### EVE CLASS ###", cpplib, line)
                line = re.sub(r"### EVE RES ###", defres, line)
                if re.findall(r"/\* ### BEGIN API ### \*/", line):
                    flag = 1
                    cppfile.extend(apidefs)
                if re.findall(r"/\* ### BEGIN API PROTO ### \*/", line):
                    flag = 2
                    cppfile.extend(apiproto)
                if re.findall(r"/\* ### BEGIN API CONST ### \*/", line):
                    flag = 3
                    cppfile.extend(apiconst)
                if re.findall(r"/\* ### BEGIN API == 1 ### \*/", line):
                    if api != 1:
                        flag = -1
                    continue
                if re.findall(r"/\* ### BEGIN API == 2 ### \*/", line):
                    if api != 2:
                        flag = -1
                    continue
                if re.findall(r"/\* ### BEGIN API == 3 or 4 ### \*/", line):
                    if api != 3 and api != 4:
                        flag = -1
                    continue
                if re.findall(r"/\* ### BEGIN API >= 2 ### \*/", line):
                    if api < 2:
                        flag = -1
                    continue
                if re.findall(r"/\* ### BEGIN API >= 3 ### \*/", line):
                    if api < 3:
                        flag = -1
                    continue
                if re.findall(r"/\* ### BEGIN API < 5 ### \*/", line):
                    if api >= 5:
                        flag = -1
                    continue
                if re.findall(r"/\* ### BEGIN API >= 5 ### \*/", line):
                    if api < 5:
                        flag = -1
                    continue
                if re.findall(r"/\* ### END API ### \*/", line): 
                    flag = 0
                    continue
                if flag == 0: 
                    cppfile.append(line.rstrip())

            with open(file_out, "w") as file:
                for line in cppfile:
                    file.write(line + "\n")
        except:
            # File is binary or could not be parsed
            with open(file_in, "rb") as file:
                bdata = file.read()
            with open(file_out, "wb") as file:
                file.write(bdata)

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
dist_source_files.append((os.path.join(src_api,"LICENSE"), os.path.join(dest_lib,"LICENSE.txt")))

# Copy API source and header files
try:
    os.makedirs(dest_lib, exist_ok=True)
    for d in dist_source_files:
        srcf, destf = d
        print(f"{srcf} -> {destf}")
        template(srcf, destf, str_lib_name, eve_api, eve_sub_api, str_full_version, str_api_version, "", "", "")
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
template_files.append(("test.ino.template", os.path.join(test_dir,"test.ino")))
for t in template_files:
    srcf, destf = t
    print(f"{srcf} -> {destf}")
    template(srcf, destf, str_lib_name, eve_api, eve_sub_api, str_full_version, str_api_version, "", "", "")

# Command line for preprocessor
cppcmd = ['cpp', f'-I{dest_lib}', f'-DEVE_API={eve_api}']
if eve_sub_api > 0:
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

    cppapi = []
    cppapiconsts = []
    cppapiproto = []
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
        cppapi.append(cppline)
        cppprotoline = f"{cdefl[1]} {cdefl[0]}({cppparamtext})"
        cppapiproto.append(cppprotoline)

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
                    if definep[0].startswith("ROMFONT"):
                        cppvals = ""
                        for dp in definep[2:]:
                            dp = re.sub(r"EVE_", "(uint8_t)", dp)
                            cppvals += dp
                        dpc = cppvals.count(',') + 1
                        cppline = f"    const uint8_t {definep[0]}[{dpc}] = "
                        cppline += cppvals
                        cppline += ";"
                elif definep[0].startswith("ENC_"):
                    pass
                else:
                    cppline = f"    const uint32_t {definep[0]} = {definep[2]};"
                if cppline:
                    cppapi.append(cppline)
                    cppapiconsts.append(definep[0])
    #cconstdefs = list(dict.fromkeys(cconstdefs))

def sortapi(slist):
    ordered = []
    def flib(w):
        return re.findall(r"^\w+ LIB_", w)
    def fcmd(w):
        return re.findall(r"^\w+ CMD_", w)
    def feve(w):
        return re.findall(r"^\w+ eve_", w)
    llib = list(filter(flib, slist))
    llib.sort()
    lcmd = list(filter(fcmd, slist))
    lcmd.sort()
    leve = list(filter(feve, slist))
    leve.sort()
    ordered += llib
    ordered += [""]
    ordered += lcmd
    ordered += [""]
    ordered += leve
    ordered += [""]
    for done in ordered:
        try:
            slist.remove(done)
        except:
            pass
    slist.sort()
    ordered += slist
    return ordered

def sortconst(slist):
    ordered = []
    def freg(w):
        return re.findall(r"^REG_", w)
    lreg = list(filter(freg, slist))
    lreg.sort()
    for done in lreg:
        try:
            slist.remove(done)
        except:
            pass
    slist.sort()
    ordered += slist
    ordered += [""]
    ordered += lreg
    return ordered

cppapi.sort()
cppapiproto = sortapi(cppapiproto)
cppapiconsts = sortconst(cppapiconsts)

# Library main class files (template files pass 2)
template_files = []
template_files.append(("bteve.cpp.template", os.path.join(dest_lib,f"{str_lib_name}.cpp")))
template_files.append(("bteve.h.template", os.path.join(dest_lib,f"{str_lib_name}.h")))
# README.md file
template_files.append((os.path.join(src_api, "docs", "arduino.png"), os.path.join(dest_lib,"arduino.png")))
template_files.append((os.path.join(src_api, "docs", "header1x10.png"), os.path.join(dest_lib,"header1x10.png")))
template_files.append((os.path.join(src_api, "docs", "header2x8.png"), os.path.join(dest_lib,"header2x8.png")))
template_files.append(("README.md.template", os.path.join(dest_lib,"README.md")))

for t in template_files:
    srcf, destf = t
    print(f"{srcf} -> {destf}")
    template(srcf, destf, str_lib_name, eve_api, eve_sub_api, str_full_version, str_api_version, cppapi, cppapiproto, cppapiconsts)

# Make the examples directory
examples_dir = os.path.join(dest_lib, "examples")
os.makedirs(examples_dir, exist_ok=True)

# Copy all the examples
example_files = []
for path, subdirs, files in os.walk(os.path.normpath("examples")):
    for dir in subdirs:
        destdir = dir + "_EVE" + str_full_version
        # Make a unique destination directory
        os.makedirs(os.path.join(dest_lib, path, destdir), exist_ok=True)
    for name in files:
        # Rename the destination file path
        destpath = path + "_EVE" + str_full_version
        destname = name
        # Rename the sketch main file
        if (os.path.basename(name) == os.path.split(path)[-1] + ".ino"):
            destname = os.path.split(path)[-1] + "_EVE" + str_full_version + ".ino"
        example_files.append((os.path.join(path, name), os.path.join(dest_lib, destpath, destname)))

for t in example_files:
    srcf, destf = t
    print(f"{srcf} -> {destf}")
    template(srcf, destf, str_lib_name, eve_api, eve_sub_api, str_full_version, str_api_version, "", "", "")
