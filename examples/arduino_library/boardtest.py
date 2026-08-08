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

The following boards have successfully compiled the simple example:

- Adafruit Circuit Playground (arduino:avr:circuitplay32u4cat)
- Adafruit Circuit Playground Express (arduino:samd:adafruit_circuitplayground_m0)
- Arduino BT (arduino:avr:bt)
- Arduino Due (Native USB Port) (arduino:sam:arduino_due_x)
- Arduino Due (Programming Port) (arduino:sam:arduino_due_x_dbg)
- Arduino Duemilanove or Diecimila (arduino:avr:diecimila)
- Arduino Esplora (arduino:avr:esplora)
- Arduino Ethernet (arduino:avr:ethernet)
- Arduino Fio (arduino:avr:fio)
- Arduino Giga R1 (arduino:zephyr_main:giga)
- Arduino Industrial 101 (arduino:avr:chiwawa)
- Arduino Leonardo (arduino:avr:leonardo)
- Arduino Leonardo ETH (arduino:avr:leonardoeth)
- Arduino M0 (arduino:samd:mzero_bl)
- Arduino M0 Pro (Native USB Port) (arduino:samd:mzero_pro_bl)
- Arduino M0 Pro (Programming Port) (arduino:samd:mzero_pro_bl_dbg)
- Arduino MKR 1000 WiFi (arduino:samd:mkr1000)
- Arduino MKR FOX 1200 (arduino:samd:mkrfox1200)
- Arduino MKR GSM 1400 (arduino:samd:mkrgsm1400)
- Arduino MKR NB 1500 (arduino:samd:mkrnb1500)
- Arduino MKR Vidor 4000 (arduino:samd:mkrvidor4000)
- Arduino MKR WAN 1300 (arduino:samd:mkrwan1300)
- Arduino MKR WAN 1310 (arduino:samd:mkrwan1310)
- Arduino MKR WiFi 1010 (arduino:samd:mkrwifi1010)
- Arduino MKR Zero (arduino:samd:mkrzero)
- Arduino Mega ADK (arduino:avr:megaADK)
- Arduino Mega or Mega 2560 (arduino:avr:mega)
- Arduino Micro (arduino:avr:micro)
- Arduino Mini (arduino:avr:mini)
- Arduino NANO 33 IoT (arduino:samd:nano_33_iot)
- Arduino Nano (arduino:avr:nano)
- Arduino Nano 33 BLE (arduino:zephyr_main:nano33ble)
- Arduino Nano ESP32 (arduino:esp32:nano_nora)
- Arduino Nano Every (arduino:megaavr:nona4809)
- Arduino Nano Matter (arduino:zephyr_main:nano_matter)
- Arduino Nano Matter (SiliconLabs:silabs:nano_matter)
- Arduino Nano R4 (arduino:renesas_uno:nanor4)
- Arduino Nano RP2040 Connect (arduino:zephyr_main:nano_connect)
- Arduino Nicla Vision (arduino:zephyr_main:nicla_vision)
- Arduino Portenta C33 (arduino:renesas_portenta:portenta_c33)
- Arduino Portenta C33 (arduino:zephyr_main:portentac33)
- Arduino Portenta H7 (arduino:zephyr_main:portentah7)
- Arduino Portenta H7 (arduino:mbed_portenta:envie_m7)
- Arduino Primo (arduino:nrf52:primo)
- Arduino Primo Core (arduino:nrf52:primo_core)
- Arduino Pro or Pro Mini (arduino:avr:pro)
- Arduino Robot Control (arduino:avr:robotControl)
- Arduino Robot Motor (arduino:avr:robotMotor)
- Arduino Tian (arduino:samd:tian)
- Arduino UNO (arduino:avr:uno)
- Arduino UNO Mini (arduino:avr:unomini)
- Arduino UNO Q (arduino:zephyr:unoq)
- Arduino UNO R4 Minima (arduino:renesas_uno:minima)
- Arduino UNO R4 WiFi (arduino:renesas_uno:unor4wifi)
- Arduino UNO WiFi (arduino:avr:unowifi)
- Arduino Uno WiFi Rev2 (arduino:megaavr:uno2018)
- Arduino Yún (arduino:avr:yun)
- Arduino Yún Mini (arduino:avr:yunmini)
- Arduino Zero (Native USB Port) (arduino:samd:arduino_zero_native)
- Arduino Zero (Programming Port) (arduino:samd:arduino_zero_edbg)
- Ezurio Lyra 24P 20dBm Dev Kit (SiliconLabs:silabs:lyra24p20)
- LilyPad Arduino (arduino:avr:lilypad)
- LilyPad Arduino USB (arduino:avr:LilyPadUSB)
- Linino One (arduino:avr:one)
- Seeed Studio XIAO MG24 (Sense) (SiliconLabs:silabs:xiao_mg24)
- Silicon Labs BGM220 Explorer Kit (SiliconLabs:silabs:bgm220explorerkit)
- Silicon Labs xG24 Dev Kit (SiliconLabs:silabs:xg24devkit)
- Silicon Labs xG24 Explorer Kit (SiliconLabs:silabs:xg24explorerkit)
- Silicon Labs xG27 Dev Kit (SiliconLabs:silabs:xg27devkit)
- SparkFun Thing Plus Matter (SiliconLabs:silabs:thingplusmatter)

The following boards do not successfully compile:

- Arduino NG or older (arduino:avr:atmegang) - Insufficient program space
- Arduino Gemma (arduino:avr:gemma) - No Serial class
- Arduino Nicla Sense ME (arduino:zephyr_main:nicla_sense) - No SPI class
- Arduino Opta (arduino:zephyr_main:opta) - No SPI class
- Arduino Portenta X8 (arduino:mbed_portenta:portenta_x8) - No Serial class

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
    "arduino:avr:gemma", # "Arduino Gemma" - No Serial class
    "arduino:zephyr_main:nicla_sense", # "Arduino Nicla Sense ME" - No SPI class
    "arduino:mbed_portenta:portenta_x8", # "Arduino Portenta X8" - No Serial class
    "arduino:zephyr_main:opta", # "Arduino Opta" - No SPI class
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


