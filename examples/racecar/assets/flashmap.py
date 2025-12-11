import sys
import os
import re

if len(sys.argv) > 2:
    mapfile = sys.argv[1]
    cfile = sys.argv[2]
else:
    print("Please specify a flash map file and the C file")
    exit(-1)

print(f"Setting map offsets and sizes from \"{mapfile}\" to \"{cfile}\"")
with open(mapfile, "r") as mapf:
    maplines = [line.rstrip() for line in mapf]
    maps = []
    for line in maplines:
        maps.append([col.strip() for col in line.split(":")])

    with open(cfile, "r") as cf:
        clines = [line for line in cf]

    pattern = re.compile(r"/\* (\w+) (.+) \*/\s*[0-9]+s*")

    with open(cfile, "w") as cf:
        # Parse the C file and find /* (offset|size) <file> */
        for cline in clines:
            m = pattern.search(cline)
            if m:
                # Find type and filename
                type = m.group(1)
                fname = m.group(2)
                if type and fname:
                    for map in maps:
                        if fname == map[0]:
                            if type == "offset": val = map[1]
                            if type == "size": val = map[2]
                            cline = cline[:m.start()] + f"/* {type} {fname} */ {val} " + cline[m.end():]
            cf.write(cline)
