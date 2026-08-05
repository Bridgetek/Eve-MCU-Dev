# Test installed Arduino boards against the generated libraries
import subprocess
import os
import re
import shutil

"""
Commands needed to do this:

> arduino-cli board listall
Board Name                          FQBN
Adafruit Circuit Playground         arduino:avr:circuitplay32u4cat
Arduino UNO                         arduino:avr:uno

> arduino-cli sketch new -f simple_EVE5

> arduino-cli compile -b arduino:avr:uno --build-property compiler.cpp.extra_flags="-DFT8XX_TYPE=BT820 -DDISPLAY_RES=WXUGA"

"""
level = 0
verbose = 0

# Get a list of Arduino Boards to test
def get_boards(exclude, verbose=0):
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
                    if e in name:
                        include = False
                if include:
                    if verbose: print(f"Adding {name} as {fqbn} (rank {rank})")
                    found_boards.append({"name": name, "fqbn": fqbn, "rank": rank})
                else:
                    if verbose: print(f"Excluding {name} as {fqbn}")
    except:
        raise Exception(f"The arduino-cli board listall command failed.")
    return found_boards

def get_libraries(verbose=0):
    found_libraries = []
    prefix = "Bridgetek_EVE"
    apis = ["1", "2_1", "2_2", "3", "4", "5"]
    for a in apis:
        if os.path.exists(prefix + a):
            if verbose: print(f"Adding library {prefix + a}")
            found_libraries.append(prefix + a)
    return found_libraries

def get_examples(libraries, verbose=0):
    found_examples = []
    for l in libraries:
        exd = os.path.join(l, "examples")
        dirnames = [d for d in os.listdir(exd) if os.path.isdir(os.path.join(exd, d))]
        for d in dirnames:
            if verbose: print(f"Adding example {d} to {l}")
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
    return cliret.returncode

# Arduino boards to exclude from testing
exclude_boards = [
    "Arduino NG or older", # Too small a memory
    "Arduino Gemma", # Does not include Serial
    "Arduino Portenta X8", # Requires SerialRPC
]

# Example and Arduino core types to skip
exclude_example_board = [
    {"src": "b2tf", "fqbn": "arduino:sam:"},
    {"src": "medinfo", "fqbn": "arduino:avr:"},
]

# Examples to compile, library paths to use and compiler flags
libraries = get_libraries()
examples = get_examples(libraries)
boards = get_boards(exclude_boards)

ex_count = 0
brd_count = 0
rank_count = 0

print(f"There are {len(examples)} examples for each board.")
print(f"Testing the following boards from a total of {len(boards)}:")
for brd in boards:
    if brd["rank"] <= level:
        print(f"- {brd["name"]} ({brd["fqbn"]})")

for ex in examples:
    for brd in boards:
        if brd["rank"] <= level:
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


