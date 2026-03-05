# Typically run with "python preprocess.py --dist ../EVE-MCU-Dev --api 5"
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
parser.add_argument("--dest", help="destination directory for Arduino library (default is Bridgetek_EVE<API>)")
parser.add_argument("--api", default=eve_api, help="EVE API to build library for (valid values are 1 to 5)")
parser.add_argument("--apisub", default=eve_sub_api, help="EVE SUB API to build library for (for EVE API 2 must be 1 or 2)")
parser.add_argument("--ver", required=True, help="Arduino library version number")
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
def template(file_in, file_out, ardver, cpplib, api, subapi, str_full_version, apidefs, apiimpl, apiproto, apiconstlist, definelist, excludelist, apirefactor):
    cppfile = []
    flag = 0
    str_full_url = re.sub(r"_", '-', str_full_version)
    
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

    # Preprocess and replace markers with values
    with open(file_in, "r") as file:
        try:
            nest = []
            while line := file.readline():
                cppadd = []
                line = line.rstrip()
                # Modify markers in templates
                line = re.sub(r"### EVE API VER ###", str_full_version, line)
                line = re.sub(r"### EVE API URL ###", str_full_url, line)
                line = re.sub(r"### EVE API ###", f"{api}", line)
                line = re.sub(r"### EVE SUB API ###", f"{subapi}", line)
                line = re.sub(r"### EVE DEV ###", apidev, line)
                line = re.sub(r"### EVE LIB NAME ###", apilib, line)
                line = re.sub(r"### EVE CLASS ###", cpplib, line)
                line = re.sub(r"### EVE RES ###", defres, line)
                line = re.sub(r"### ARDUINO VERSION ###", ardver, line)
                # Replace template areas
                if re.findall(r"### API HEADER ###", line):
                    flag = 1
                    cppfile.extend(apidefs)
                if re.findall(r"### API IMPLEMENTATION ###", line):
                    flag = 1
                    cppfile.extend(apiimpl)
                if re.findall(r"### API PROTO ###", line):
                    flag = 1
                    cppfile.extend(apiproto)
                if re.findall(r"### API CONST ###", line):
                    flag = 1
                    cppfile.extend(apiconstlist)
                # Global static consts moved into PROGMEM storage on Arduino
                line = re.sub(r"^static const uint8_t\s*(\w+)\s*\[", r"PROGMEM static const uint8_t \g<1> [", line)
                line = re.sub(r"^const uint8_t\s*(\w+)\s*\[", r"PROGMEM const uint8_t \g<1> [", line)
                # Change code to use C++ class instead of C library
                if apirefactor:
                    # Add in eve.setup before eve.Init
                    line = re.sub(r"^(\s*)EVE_Init\s*\(\s*\)\s*;", r"\g<1>// Setup the EVE display (" + defres + ")\n" \
                                                      r"\g<1>" "eve.setup(DISPLAY_RES);\n" \
                                                      r"\g<1>" "// Setup the EVE library\n" \
                                                      r"\g<1>" "eve.Init();\n" \
                        , line)
                    # General replace of EVE_ with eve. (except on preprocessor lines)
                    # when it matches an entry in the 
                    if not line.strip().startswith("#"):
                        subline = ""

                        line = re.sub(r"\beve_loadpatch\b", "patch_eve_loadpatch", line)

                        token = re.search(r"\bEVE_[\w]+\b", line)
                        while token:
                            skiptoken = False
                            scrambletoken = False
                            subtoken = token.group(0)[4:]

                            # Check if the token is part of the API base patch
                            if os.path.basename(file_in).startswith("patch_"):
                                if re.match("CMD_(END|)(REGION|TOUCHOFFSET)", subtoken):
                                    # Rename and hence ignore base patch symbols
                                    scrambletoken = True
                                if re.match("eve_loadpatch", subtoken):
                                    # Rename and hence ignore base patch symbols
                                    scrambletoken = True

                            # If the token is a constant definition usually beginning with OPT_ then check it matches with the API defined constants
                            if apiconstlist:
                                # If it is not part of the API (in the list) then do not replace it...
                                if not subtoken in apiconstlist:
                                    skiptoken = True
                            # If the token is a CMD_... then check if it matches with an API defined function
                            if apiproto:
                                foundproto = False
                                for a in apiproto:
                                    if re.search(r"\s" + subtoken + r"\(", a):
                                        # If it is in the API list then replace it...
                                        foundproto = True
                                if not foundproto:
                                    skiptoken = True

                            if scrambletoken:
                                # Note replacement is made to ignore this in code
                                subline += line[:token.start()] + "PATCH_" + subtoken
                            elif skiptoken:
                                # Keep the token intact
                                subline += line[:token.end()]
                            else:
                                # Note replacement is made to use class version
                                subline += line[:token.start()] + "eve." + subtoken
                            line = line[token.end():]
                            token = re.search(r"\bEVE_[\w]+\b", line)
                        line = subline + line
                    # Change references to the FONT header structures to class definitions
                    line = re.sub(r"\beve\.(GPU_\w+)\b", "Bridgetek_EVE" + str_full_version + r"::\g<1>", line)
                    # Change references to the ROMFONT array to point to class member
                    line = re.sub(r"\beve\.(ROMFONT_\w+)\b", "Bridgetek_EVE" + str_full_version + r"::\g<1>", line)
                    # Rename refereces to EVE_DISP_WIDTH/HEIGHT to class member
                    line = re.sub(r"\bEVE_(DISP_\w+)\b", r"eve.\g<1>()", line)
                    # Add extern or definition of the EVE class
                    extern = ""
                    if not file_out.endswith("eve_example.ino"):
                        extern = "extern "
                    line = re.sub(r"#include \<EVE.h\>", "#include <Bridgetek_EVE" + str_full_version + ".h>" \
                                                          "\n\n/**\n" \
                                                          " @brief EVE library handle.\n" \
                                                          " @details This is the one instance of the EVE library. Available as a global.\n" \
                                                          " */\n" \
                                                          + extern + "Bridgetek_EVE" + str_full_version + " eve;\n" \
                                  , line)
                    # Remove snippet top level directory references 
                    line = re.sub(r"#include \"[\w]+/([\w.]+)\"", r"#include \"\g<1>\"", line)
                    # Change references to const array stored in class to pointer from unsized array
                    line = re.sub(r"([\w.]+)\[\] = (Bridgetek_EVE[0-9_]*::ROMFONT_)", r"*\g<1> = \g<2>", line)
                else:
                    # Update name of library header file to prevent clashes
                    line = re.sub(r"#include \<EVE.h\>", "#include <EVE" + str_full_version + ".h>", line)

                if flag == 0:
                    # Simulate preprocessor by excluding/replacing code
                    match_ifsubapi = re.match(r"\s*#(if|elif)\s+IS_EVE_SUB_API\((.+)\)", line)
                    match_ifapi = re.match(r"\s*#(if|elif)\s+IS_EVE_API\((.+)\)", line)
                    match_if = re.match(r"\s*#(if|elif|ifdef|ifndef)\s+", line)
                    match_else = re.match(r"\s*#else\s*", line)
                    match_endif = re.match(r"\s*#endif\s*", line)
                    match_define = re.match(r"(\s*#define\s*)(\w+)", line)
                    match_include = re.match(r"\s*#include\s*\"(.+)\"", line)

                    try:
                        if match_ifsubapi:
                            cov = []
                            if match_ifsubapi.group(1) == "elif":
                                (pos, _, cov) = nest.pop()
                                cov.extend(pos)
                            pos = []
                            for lev in "".join(match_ifsubapi.group(2).split()).split(","):
                                pos.append(-int(lev))
                            nest.append((pos, True, cov))
                            print(nest)
                            flag = -1
                        elif match_ifapi:
                            cov = []
                            if match_ifapi.group(1) == "elif":
                                (pos, _, cov) = nest.pop()
                                cov.extend(pos)
                            pos = []
                            for lev in "".join(match_ifapi.group(2).split()).split(","):
                                pos.append(int(lev))
                            nest.append((pos, True, cov))
                            flag = -1
                        elif match_else:
                            (pos, _, cov) = nest.pop()
                            if cov != None:
                                if pos != None:
                                    cov.extend(pos)
                                flag = -1
                                nest.append((cov, False, None))
                            else:
                                nest.append((cov, True, None))
                        elif match_endif:
                            (pos, _, cov) = nest.pop()
                            if pos != None:
                                flag = -1
                        elif match_if:
                            if match_if.group(1) == "elif":
                                nest.pop()
                            nest.append((None, True, None))

                        depth = len(nest)
                        if depth > 0:
                            for n in nest:
                                (cond,sense,_) = n
                                if sense:
                                    if cond:
                                        if not api in cond and not (-subapi) in cond:
                                            flag = -1
                                else:
                                    if cond:
                                        if api in cond or (-subapi) in cond:
                                            flag = -1
                        if definelist:
                            if match_define:
                                for (d,v) in definelist:
                                    if d == match_define.group(2):
                                        print(f"Setting {d} = {v}")
                                        line = match_define.group(1) + match_define.group(2) + " " + v
                        if excludelist:
                            if match_include:
                                if match_include.group(1) in excludelist:
                                    flag = -1

                    except Exception as inst:
                        print("Error: preprocessing - ", inst)
                        exit(0)
                   
                if flag == 0: 
                    # Use variable EVE_RAM_G_SIZE for size of RAM_G
                    if file_out.endswith("BT82x.h"):
                        line = re.sub(r"EVE_RAM_G_CONFIG_SIZE", "EVE_RAM_G_SIZE", line)
                    # Add PROGMEM storage read for patch_base.c
                    elif file_out.endswith("patch_base.c"):
                        if line == "#include \"patch_base.h\"":
                            cppadd = [
                                    "#include <string.h>",
                                    "",
                                    "#if defined(ESP8266) || defined(ESP32)",
                                    "#include <pgmspace.h>",
                                    "#else",
                                    "#include <avr/pgmspace.h>",
                                    "#endif",
                                    "",
                            ]
                            print("patch_base.c updated for PROGMEM")
                        else:
                            match = re.match(r"^(\s*)EVE_LIB_WriteDataToCMD\((\w+),\s(\w+)\);", line)
                            if match:
                                dlen = int(match.group(3))
                                line = "/*hello*/"
                                cppadd = [
                                        f"{match.group(1)}/* Read the data from the program memory into CMD. */",
                                        f"{match.group(1)}uint8_t pgm[16];",
                                        f"{match.group(1)}uint32_t pgmoffset, pgmchunk;",
                                        f"{match.group(1)}for (pgmoffset = 0; pgmoffset < {dlen}; pgmoffset += 16)",
                                        f"{match.group(1)}{{",
                                        f"{match.group(1)}    // Maximum of pgm buffer",
                                        f"{match.group(1)}    uint32_t chunk = sizeof(pgm);",
                                        f"{match.group(1)}    if (pgmoffset + chunk > {dlen})",
                                        f"{match.group(1)}    {{",
                                        f"{match.group(1)}        chunk = {dlen} - pgmoffset;",
                                        f"{match.group(1)}    }}",
                                        f"{match.group(1)}    // Load the pgm buffer",
                                        f"{match.group(1)}    memcpy_P(pgm, &{match.group(2)}[pgmoffset], chunk);",
                                        f"{match.group(1)}    EVE_LIB_WriteDataToCMD(pgm, chunk);",
                                        f"{match.group(1)}}}",
                                ]
                                print("patch_base.c updated for accessing PROGMEM")

                    if line != None:
                        cppfile.append(line)

                    for a in cppadd:
                        cppfile.append(a)

                flag = 0

            with open(file_out, "w") as file:
                for line in cppfile:
                    file.write(line + "\n")

        except Exception as inst:
            print("Error: default file handling -", inst)
            # File is binary or could not be parsed
            with open(file_in, "rb") as file:
                bdata = file.read()
            with open(file_out, "wb") as file:
                file.write(bdata)
            exit(0)

# Collate header files needed (from include directory)
dist_inc_files = ["HAL.h", "MCU.h", "FT8xx.h"]
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
dist_source_files.append((os.path.join(src_api,"include","EVE.h"), os.path.join(dest_lib,f"EVE{str_full_version}.h")))
for d in dist_inc_files:
    dist_source_files.append((os.path.join(src_api,"include",d), os.path.join(dest_lib,d)))
dist_source_files.append((os.path.join(src_api,"source","EVE_API.c"), dest_api))
dist_source_files.append((os.path.join(src_api,"source","EVE_HAL.c"), os.path.join(dest_lib,"EVE_HAL.c")))
dist_source_files.append((os.path.join(src_api,"ports","eve_arch_arduino","eve_arch_arduino.ino"), os.path.join(dest_lib,"EVE_MCU.cpp")))
if eve_api == 5:
    dist_source_files.append((os.path.join(src_api,"ports","eve_bt82x","patch_base.c"), os.path.join(dest_lib,"patch_base.c")))
    dist_source_files.append((os.path.join(src_api,"ports","eve_bt82x","patch_base.h"), os.path.join(dest_lib,"patch_base.h")))
dist_source_files.append((os.path.join(src_api,"LICENSE"), os.path.join(dest_lib,"LICENSE.txt")))

# Copy API source and header files
try:
    os.makedirs(dest_lib, exist_ok=True)
    for d in dist_source_files:
        srcf, destf = d
        print(f"API: {srcf} -> {destf}")
        template(srcf, destf, args.ver, str_lib_name, eve_api, eve_sub_api, str_full_version, "", "", "", "", None, None, False)
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
    print(f"template: {srcf} -> {destf}")
    template(srcf, destf, args.ver, str_lib_name, eve_api, eve_sub_api, str_full_version, "", "", "", "", None, None, False)

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

    cppapihdrdef = []
    cppapihdrdec = []
    cppapiimpldef = []
    cppapiconsts = []
    cppapiconstslist = []
    cppapiproto = []
    cppapienum = []
    for line in cfndefs:
        cdefl = cppre.split(line)
        # cdefl[0] = ""
        # cdefl[1] = "void|int.*|uint.*|char"
        # cdefl[2] = "EVE_\w*"
        # cdefl[3] = "", "void", "uint32_t options", or "..."
        # cdefl[4] = ""
        if cdefl[2] == "EVE_NOP":
            # Don't make a functino called NOP()!
            continue
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
        elif cdefl[0].startswith("eve_") and not cdefl[1].startswith("void"):
            addnret = "return "
        cppline = f"    {cdefl[1]} {cdefl[0]}({cppparamtext}) {{ {addnvariac}{addnret}::{cdefl[2]}({cparamtext}); {addevariac}}};"
        cppapihdrdef.append(cppline)
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
                        #static const uint8_t ROMFONT_WIDTHS[32];
                        cppline = f"    static const uint8_t {definep[0]}[{dpc}];"
                        cppapihdrdec.append(cppline)
                        #constexpr uint8_t Bridgetek_EVE1::ROMFONT_WIDTHS[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,8,10,13,14,17,24,30,12,16,18,22,28,36};
                        cppline = f"constexpr uint8_t Bridgetek_EVE{str_full_version}::{definep[0]}[] = {cppvals};"
                        cppapiimpldef.append(cppline)
                        cppapiconstslist.append(definep[0])
                elif definep[0].startswith("ENC_"):
                    pass
                elif definep[0].startswith("RAM_G_CONFIG_SIZE"):
                    pass
                elif definep[0].startswith("RAM_G_SIZE"):
                    pass
                else:
                    cppline = f"      {definep[0]} = {definep[2]},"
                    cppapiconsts.append(cppline)
                    cppapiconstslist.append(definep[0])

def sortapi(slist):
    ordered = []
    def flib(w):
        return re.findall(r"^\s*\w+ LIB_", w)
    def fcmd(w):
        return re.findall(r"^\s*\w+ CMD_", w)
    def fconst(w):
        return re.findall(r"^\s*const ", w)
    def frem(w):
        return not(fcmd(w) or flib(w) or fconst(w))
    llib = list(filter(flib, slist))
    llib.sort()
    lcmd = list(filter(fcmd, slist))
    lcmd.sort()
    lrem = list(filter(frem, slist))
    lrem.sort()
    lconst = list(filter(fconst, slist))
    lconst.sort()
    ordered += llib
    if len(lcmd) and len(ordered): ordered += [""]
    ordered += lcmd
    if len(lrem) and len(ordered): ordered += [""]
    ordered += lrem
    if len(lconst) and len(ordered): ordered += [""]
    ordered += lconst
    return ordered

def sortconst(slist):
    ordered = []
    def freg(w):
        return re.findall(r"^\s*REG_", w)
    def fapi(w):
        return (re.findall(r"_", w) and not freg(w))
    def frem(w):
        return not(freg(w) or fapi(w))
    lreg = list(filter(freg, slist))
    lreg.sort()
    lapi = list(filter(fapi, slist))
    lapi.sort()
    lrem = list(filter(frem, slist))
    lrem.sort()
    ordered += lapi
    if len(lrem) and len(ordered): ordered += [""]
    ordered += lrem
    if len(lreg) and len(ordered): ordered += [""]
    ordered += lreg
    return ordered

cppapihdrdef = sortapi(cppapihdrdef)
cppapihdrdec = sortapi(cppapihdrdec)
cppapiimpldef = sortapi(cppapiimpldef)
cppapiproto = sortapi(cppapiproto)
cppapiconsts = sortconst(cppapiconsts)
cppapiconstslist = sortconst(cppapiconstslist)

cppapienum.append("")
cppapienum.append("    enum {")
for c in cppapiconsts:
    cppapienum.append(c)
cppapienum.append("    };")

cppapiheader = []
cppapiheader.extend(cppapienum)
if len(cppapihdrdef) and len(cppapiheader): cppapiheader += [""]
cppapiheader.extend(cppapihdrdef)
if len(cppapihdrdec) and len(cppapiheader): cppapiheader += [""]
cppapiheader.extend(cppapihdrdec)

# Library main class files (template files pass 2)
template_files = []
# Library graphics / binary files
graphics_files = []

template_files.append(("bteve.cpp.template", os.path.join(dest_lib,f"{str_lib_name}.cpp")))
template_files.append(("bteve.h.template", os.path.join(dest_lib,f"{str_lib_name}.h")))
# README.md file
template_files.append(("README.md.template", os.path.join(dest_lib,"README.md")))

for t in template_files:
    srcf, destf = t
    print(f"class: {srcf} -> {destf}")
    template(srcf, destf, args.ver, str_lib_name, eve_api, eve_sub_api, str_full_version, cppapiheader, cppapiimpldef, cppapiproto, cppapiconstslist, None, None, False)

# Make the examples directory
examples_dir = os.path.join(dest_lib, "examples")
os.makedirs(examples_dir, exist_ok=True)

# Copy all the examples
for subdirs in os.scandir(os.path.normpath("examples")):
    if subdirs.is_dir():
        exampledir = subdirs.name
        examplesupported = True
        supportslist = os.path.normpath(os.path.join("examples", exampledir, "!supports.txt"))
        if os.path.exists(supportslist):
            examplesupported = False
            with open(supportslist, "r") as file:
                while d := file.readline():
                    d = d.strip()
                    if not d.startswith("#"):
                        support = re.findall(r'\s|,|[^,\s]+', d)
                        for s in support:
                            if not s.startswith("#"):
                                try:
                                    if eve_api == int(s):
                                        examplesupported = True
                                except:
                                    pass
                            else:
                                break
        if examplesupported:
            print("Supporting example:", exampledir)
            destdir = exampledir + "_EVE" + str_full_version
            # Make a unique destination directory
            os.makedirs(os.path.join(dest_lib, "examples", destdir), exist_ok=True)
            # Add a docs directory
            os.makedirs(os.path.join(dest_lib, "examples", destdir, "docs"), exist_ok=True)
            
            defines_list = []
            defineslist = os.path.normpath(os.path.join("examples", exampledir, "!defines.txt"))
            if os.path.exists(defineslist):
                with open(defineslist, "r") as file:
                    while d := file.readline():
                        d = d.strip()
                        if not d.startswith("#"):
                            dw = d.split()
                            if len(dw) > 0: 
                                defname = dw[0].strip()
                                if len(dw) > 1: 
                                    defval = dw[1].strip()
                                else:
                                    defval = ""
                                defines_list.append((defname, defval))
            
            # Copy any docs
            docdir = os.path.normpath(os.path.join(src_api, "examples", exampledir, "docs"))
            docfiles = [f for f in os.listdir(docdir) if os.path.isfile(os.path.join(docdir, f))]
            for d in docfiles:
                print(f"Adding example doc file {d}")
                graphics_files.append((os.path.join(docdir, d), os.path.join(dest_lib, "examples", destdir, "docs", d)))

            # Copy project files
            exclude_files = []
            excludelist = os.path.normpath(os.path.join("examples", exampledir, "!excludes.txt"))
            if os.path.exists(excludelist):
                with open(excludelist, "r") as file:
                    while d := file.readline():
                        d = d.strip()
                        if not d.startswith("#"):
                            exclude_files.append(d)
            commondir = os.path.normpath(os.path.join(src_api, "examples", exampledir, "common"))
            commonfiles = [f for f in os.listdir(commondir) if os.path.isfile(os.path.join(commondir, f))]
            # Example source files
            common_files = []
            for d in commonfiles:
                if d in exclude_files:
                    print(f"Ignoring excluded file {d}")
                    continue
                if d.startswith("!"):
                    print(f"Ignoring direction file {d}")
                    continue
                if os.path.splitext(d)[1] == ".bin":
                    print(f"Ignoring binary file {d}")
                    continue
                if os.path.splitext(d)[1] == ".c":
                    destname = os.path.splitext(d)[0] + ".ino"
                else:
                    destname = d
                print(f"Adding example common file {d} renamed to {destname}")
                common_files.append((os.path.join(commondir, d), os.path.join(dest_lib, "examples", destdir, destname)))

            snippetdir = os.path.normpath(os.path.join(src_api, "examples", "snippets"))
            snippetlist = os.path.normpath(os.path.join("examples", exampledir, "!snippets.txt"))
            if os.path.exists(snippetlist):
                with open(snippetlist, "r") as file:
                    while d := file.readline():
                        d = d.strip()
                        if not d.startswith("#"):
                            if os.path.splitext(d)[1] == ".c":
                                destname = os.path.splitext(d)[0] + ".ino"
                            else:
                                destname = d
                            #destname = os.path.basename(destname)
                            print(f"Adding snippet file {d} renamed to {destname}")
                            common_files.append((os.path.join(snippetdir, d), os.path.join(dest_lib, "examples", destdir, os.path.basename(destname))))

            assetdir = os.path.normpath(os.path.join(src_api, "examples", exampledir, "assets"))
            assetlist = os.path.normpath(os.path.join("examples", exampledir, "!assets.txt"))
            if os.path.exists(assetlist):
                with open(assetlist, "r") as file:
                    while d := file.readline():
                        d = d.strip()
                        if not d.startswith("#"):
                            if os.path.splitext(d)[1] == ".c":
                                destname = os.path.splitext(d)[0] + ".ino"
                            else:
                                destname = d
                            #destname = os.path.basename(destname)
                            print(f"Adding asset file {d} renamed to {destname}")
                            common_files.append((os.path.join(assetdir, d), os.path.join(dest_lib, "examples", destdir, os.path.basename(destname))))

            for t in common_files:
                srcf, destf = t
                print(f"common example files: {srcf} -> {destf}")
                template(srcf, destf, args.ver, str_lib_name, eve_api, eve_sub_api, str_full_version, "", "", cppapiproto, cppapiconstslist, defines_list, exclude_files, True)

            # Example files
            example_files = []
            # Definitions
            example_defs = []

            examplefiles = os.scandir(os.path.normpath(os.path.join("examples", exampledir)))
            for examplefile in examplefiles:
                if examplefile.is_file():
                    examplefilename = examplefile.name
                    if examplefilename.startswith("!"):
                        print(f"Ignoring direction file {examplefilename}")
                        continue

                    # Rename the destination file path
                    destpath = exampledir + "_EVE" + str_full_version
                    destname = examplefilename
                    if os.path.splitext(examplefilename)[1] == ".png" or os.path.splitext(examplefilename)[1] == ".jpg":
                        graphics_files.append((os.path.join("examples", exampledir, examplefilename), os.path.join(dest_lib, "examples", destpath, destname)))
                    else:
                        # Rename the sketch main file
                        if (os.path.basename(examplefilename) == os.path.split(exampledir)[-1] + ".ino"):
                            destname = os.path.split(exampledir)[-1] + "_EVE" + str_full_version + ".ino"
                        example_files.append((os.path.join("examples", exampledir, examplefilename), os.path.join(dest_lib, "examples", destpath, destname)))
            for t in example_files:
                srcf, destf = t
                print(f"examples: {srcf} -> {destf}")
                template(srcf, destf, args.ver, str_lib_name, eve_api, eve_sub_api, str_full_version, "", "", cppapiproto, cppapiconstslist, defines_list, None, False)
        else:
            print("Not supporting example:", exampledir)

graphics_files.append((os.path.join(src_api, "docs", "arduino.png"), os.path.join(dest_lib,"arduino.png")))
graphics_files.append((os.path.join(src_api, "docs", "header1x10.png"), os.path.join(dest_lib,"header1x10.png")))
graphics_files.append((os.path.join(src_api, "docs", "header2x8.png"), os.path.join(dest_lib,"header2x8.png")))

for t in graphics_files:
    srcf, destf = t
    print(f"binary: {srcf} -> {destf}")
    with open(srcf, "rb") as src:
        with open(destf, "wb") as dest:
            dest.write(src.read())

