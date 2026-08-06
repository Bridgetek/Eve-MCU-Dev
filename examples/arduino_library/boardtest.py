# Test installed Arduino boards against the generated libraries
import subprocess
import os
import re
import shutil
import argparse

"""
Commands needed to do this:

> arduino-cli board listall
Board Name                          FQBN
Adafruit Circuit Playground         arduino:avr:circuitplay32u4cat
Arduino UNO                         arduino:avr:uno

> arduino-cli sketch new -f simple_EVE5

> arduino-cli compile -b arduino:avr:uno --build-property compiler.cpp.extra_flags="-DFT8XX_TYPE=BT820 -DDISPLAY_RES=WXUGA"

"""
level = 1
verbose = 0

# Default API version
eve_api = None
eve_sub_api = None

parser = argparse.ArgumentParser(description="Arduino Board Test Builder for EVE")
parser.add_argument("--exclude", help="list of FQBNs of boards to exclude")
parser.add_argument("--boards", help="list of FQBNs of specific boards to test (default all boards)")
parser.add_argument("--examples", help="list of specific examples to test (default all examples)")
parser.add_argument("--level", help="number of boards of each FQBN to test (default 1 board per chip type)")
parser.add_argument("--api", default=eve_api, help="EVE API to build library for (valid values are 1 to 5) (default all libraries)")
parser.add_argument("--apisub", default=eve_sub_api, help="EVE SUB API to build library for (for EVE API 2 must be 1 or 2)")
(args, rem) = parser.parse_known_args()
if args.api: eve_api = int(args.api)
if args.apisub: eve_sub_api = int(args.apisub)
if args.level: level = int(args.level)

# Check valid input
if (eve_api < 1) or (eve_api > 5):
    raise Exception("Invalid value for --api")
if (eve_api == 2) and ((eve_sub_api < 1) or (eve_sub_api > 2)):
    raise Exception("Invalid value for --apisub required when --api is 2")

# Get a list of Arduino Boards to test
def get_boards(exclude, specific, verbose=0):
    found_boards = []
    try:
        cliret = subprocess.run("arduino-cli board listall", stdout=subprocess.PIPE)
        clioutput = cliret.stdout.decode('utf-8')
        boardlist = clioutput.splitlines()[1:]
        for b in boardlist:
            bsplit = b.rsplit(" ", 1)
            if len(bsplit) == 2:
                name = bsplit[0].strip()
                fqbn = bsplit[1]
                foundstem = fqbn.rsplit(":", 1)[0]
                rank = 0
                for s in found_boards:
                    boardstem = s["fqbn"].rsplit(":", 1)[0]
                    if foundstem == boardstem:
                        rank += 1
                include = True
                for e in exclude:
                    if fqbn.startswith(e):
                        include = False
                if specific:
                    for s in specific:
                        if fqbn.startswith(s) == False:
                            include = False
                if include:
                    if verbose: print(f"Adding {name} as {fqbn} (rank {rank})")
                    found_boards.append({"name": name, "fqbn": fqbn, "rank": rank})
                else:
                    if verbose: print(f"Excluding {name} as {fqbn}")
    except:
        raise Exception(f"The arduino-cli board listall command failed.")
    return found_boards

def get_libraries(eve_api, eve_sub_api, verbose=0):
    found_libraries = []
    if eve_api:
        if eve_sub_api:
            apis = [f"Bridgetek_EVE{eve_api}_{eve_sub_api}"]
        else:
            apis = [f"Bridgetek_EVE{eve_api}"]
    else:
        apis = ["Bridgetek_EVE1", "Bridgetek_EVE2_1", "Bridgetek_EVE2_2", "Bridgetek_EVE3", "Bridgetek_EVE4", "Bridgetek_EVE5"]
    for a in apis:
        if os.path.exists(a):
            if verbose: print(f"Adding library {a}")
            found_libraries.append(a)
    if len(found_libraries) == 0:
        print(f"Could not find libraries")
        exit(0)
    return found_libraries

def get_examples(libraries, specific, verbose=0):
    found_examples = []
    for l in libraries:
        exd = os.path.join(l, "examples")
        dirnames = [d for d in os.listdir(exd) if os.path.isdir(os.path.join(exd, d))]
        for d in dirnames:
            include = True
            if specific:
                for s in specific:
                    if d.startswith(s) == False:
                        include = False
            if include:
                if verbose: print(f"Adding example {d} with library {l}")
                found_examples.append({"lib": l, "src": d, "flags": None})
    return found_examples

def new_sketch(lib, ex, verbose=0):
    try:
        cliret = subprocess.run(f"arduino-cli sketch new -f {ex}", stdout=subprocess.PIPE)
    except:
        raise Exception(f"The arduino-cli sketch new command failed.")
    if cliret.returncode == 0:
        if verbose: print(f"Copying from \"{lib}{os.path.sep}examples{os.path.sep}{ex}\" to \"{ex}\"")
        try:
            shutil.copytree(f"{lib}{os.path.sep}examples{os.path.sep}{ex}", ex, dirs_exist_ok=True)
        except:
            raise Exception(f"The copytree command failed.")
    return cliret.returncode

def end_sketch(ex, verbose=0):
    if verbose: print(f"Removing directory \"{ex}\"")
    try:
        #shutil.rmtree(ex)
        pass
    except:
        raise Exception(f"The rmtree command failed.")

def compile_sketch(ex, brd, flg=None, lib=None, verbose=0):
    try:
        if lib:
            libraries = "--library " + lib
        else:
            libraries = ""
        if flg:
            flags = flg
        else:
            flags = ""
        cmdline = f"arduino-cli compile -b {brd} {libraries} --build-property compiler.cpp.extra_flags=\"{flags}\""
        if verbose: print(cmdline)
        cliret = subprocess.run(cmdline, cwd=ex, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    except:
        raise Exception(f"The arduino-cli compile command failed.")
    if cliret.returncode:
        clioutput = cliret.stderr.decode('utf-8')
        lineoutput = clioutput.splitlines()
        for l in lineoutput:
            print(l)
    elif verbose:
        clioutput = cliret.stdout.decode('utf-8')
        lineoutput = clioutput.splitlines()
        for l in lineoutput:
            print(l)
        if verbose > 1:
            clioutput = cliret.stderr.decode('utf-8')
            lineoutput = clioutput.splitlines()
            for l in lineoutput:
                print(l)
    return cliret.returncode

# Arduino boards to exclude from testing
exclude_boards = [
    "arduino:avr:atmegang", # "Arduino NG or older" - Too small a memory
]
# TODO add args.exclude to exclude_boards
if args.exclude:
    for e in args.exclude.split(",;"):
        exclude_boards.append(e)

# Example and Arduino core types to skip
exclude_example_board = [
    {"src": "b2tf", "fqbn": "arduino:sam:"}, # gettimeofday linkage error
    {"src": "medinfo", "fqbn": "arduino:avr:"}, # Progmem size limited
    {"src": "racecar", "fqbn": "arduino:avr:"}, # Progmem size limited
    {"src": "medinfo", "fqbn": "arduino:megaavr:"}, # Memory size limited
    {"src": "racecar", "fqbn": "arduino:megaavr:"}, # Memory size limited
]

# Libraries to use
libraries = get_libraries(eve_api, eve_sub_api)

# Examples to compile
if args.examples:
    specific_examples = args.examples.split(",;")
else:
    specific_examples = None
examples = get_examples(libraries, specific_examples)

# Boards to test
if args.boards:
    specific_boards = args.boards.split(".;")
else:
    specific_boards = None
boards = get_boards(exclude_boards, specific_boards)

ex_count = 0
brd_count = 0
rank_count = 0

print(f"There are {len(examples)} examples for each board.")
print(f"Testing the following boards from a total of {len(boards)}:")
for brd in boards:
    if brd["rank"] < level:
        print(f"- {brd["name"]} ({brd["fqbn"]})")

for ex in examples:
    for brd in boards:
        if brd["rank"] < level:
            include = True
            for e in exclude_example_board:
                if ex["src"].startswith(e["src"]) and brd["fqbn"].startswith(e["fqbn"]):
                    include = False
            if include:
                print(f"Compiling {ex["lib"]}{os.path.sep}{ex["src"]} for {brd["name"]} ({brd["fqbn"]})")
                if new_sketch(ex["lib"], ex["src"]) == 0:
                    if compile_sketch(ex["src"], brd["fqbn"], ex["flags"], f"..{os.path.sep}{ex["lib"]}", verbose) == 0:
                        end_sketch(ex["src"])
                        pass
                    else:
                        exit(-1)


