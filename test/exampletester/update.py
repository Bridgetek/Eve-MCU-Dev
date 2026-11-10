
# Typically run with "python update.py"
# This will update the gild files for the exampletester
import os
import sys
import yaml
import subprocess

# Source directories for source files to be copied to sketch
workflow = os.path.normpath("../../.github/workflows/example_test.yml")
print(workflow)

# Copy files into sketch
try:
    if os.path.exists(workflow):
        with open(workflow, "r") as workflowf:
            flow = yaml.load(workflowf, Loader=yaml.SafeLoader)
            jobs = flow.get("jobs")
            build = jobs.get("build")
            strategy = build.get("strategy")
            matrix = strategy.get("matrix")
            actions = matrix.get("eve")
            for action in actions:
                example = action.get("EXAMPLE")
                gilded = action.get("GILDED")
                assets = action.get("ASSETS")
                eve1 = action.get("EVE1")
                eve2 = action.get("EVE2")
                eve3 = action.get("EVE3")
                eve4 = action.get("EVE4")
                eve5 = action.get("EVE5")

                if gilded == True:
                    print("Updating gilds for", example)

                    # EVE5

                    try:
                        cmd = ['cmake', '--fresh', '-B build', '-S .', '-DEVE_DEVICE=EVE_BT820', f"-DEVE_DISPLAY_RES=EVE_{eve5}", f"-DASSETS={assets}", f"-DEXAMPLE={example}"]
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("CMake failed to configure.")
                    
                    if (res.returncode != 0):
                        raise Exception(f"CMake configure error {res}.")

                    try:
                        cmd = ['cmake', '--build', 'build']
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("CMake failed to build.")
                    
                    if (res.returncode != 0):
                        raise Exception(f"CMake build error {res}.")

                    cmpout = res.stdout.decode('utf-8')
                    cmplines = cmpout.splitlines()
                    for t in cmplines:
                        t = t.strip()
                        if t.startswith("TARGET: "):
                            target = t.split(" ", 1)[1]

                    print(f"Target is \"{target}\"")

                    try:
                        cmd = [target]
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("Boottester failed to run.")

                    gild = os.path.normpath(os.path.join("gilds", f"{example}_BT820_{eve5}.txt"))
                    print("Writing gild file:", gild)

                    if (res.returncode != 0):
                        raise Exception(f"Boottester failed {res}.")
                    try:
                        with open(gild, "wb") as gildf:
                            gildf.write(res.stdout)
                    except:
                        raise Exception("Failed to write to gild file.")

                    # EVE4

                    try:
                        cmd = ['cmake', '--fresh', '-B build', '-S .', '-DEVE_DEVICE=EVE_BT818', f"-DEVE_DISPLAY_RES=EVE_{eve4}", f"-DASSETS={assets}", f"-DEXAMPLE={example}"]
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("CMake failed to configure.")
                    
                    if (res.returncode != 0):
                        raise Exception(f"CMake configure error {res}.")

                    try:
                        cmd = ['cmake', '--build', 'build']
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("CMake failed to build.")
                    
                    if (res.returncode != 0):
                        raise Exception(f"CMake build error {res}.")

                    try:
                        cmd = [target]
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("Boottester failed to run.")

                    gild = os.path.normpath(os.path.join("gilds", f"{example}_BT818_{eve4}.txt"))
                    print("Writing gild file:", gild)

                    if (res.returncode != 0):
                        raise Exception(f"Boottester failed {res}.")
                    try:
                        with open(gild, "wb") as gildf:
                            gildf.write(res.stdout)
                    except:
                        raise Exception("Failed to write to gild file.")

                    # EVE3

                    try:
                        cmd = ['cmake', '--fresh', '-B build', '-S .', '-DEVE_DEVICE=EVE_BT815', f"-DEVE_DISPLAY_RES=EVE_{eve3}", f"-DASSETS={assets}", f"-DEXAMPLE={example}"]
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("CMake failed to configure.")
                    
                    if (res.returncode != 0):
                        raise Exception(f"CMake configure error {res}.")

                    try:
                        cmd = ['cmake', '--build', 'build']
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("CMake failed to build.")
                    
                    if (res.returncode != 0):
                        raise Exception(f"CMake build error {res}.")

                    try:
                        cmd = [target]
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("Boottester failed to run.")

                    gild = os.path.normpath(os.path.join("gilds", f"{example}_BT815_{eve3}.txt"))
                    print("Writing gild file:", gild)

                    if (res.returncode != 0):
                        raise Exception(f"Boottester failed {res}.")
                    try:
                        with open(gild, "wb") as gildf:
                            gildf.write(res.stdout)
                    except:
                        raise Exception("Failed to write to gild file.")

                    # EVE2

                    try:
                        cmd = ['cmake', '--fresh', '-B build', '-S .', '-DEVE_DEVICE=EVE_FT812', f"-DEVE_DISPLAY_RES=EVE_{eve2}", f"-DASSETS={assets}", f"-DEXAMPLE={example}"]
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("CMake failed to configure.")
                    
                    if (res.returncode != 0):
                        raise Exception(f"CMake configure error {res}.")

                    try:
                        cmd = ['cmake', '--build', 'build']
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("CMake failed to build.")
                    
                    if (res.returncode != 0):
                        raise Exception(f"CMake build error {res}.")

                    try:
                        cmd = [target]
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("Boottester failed to run.")

                    gild = os.path.normpath(os.path.join("gilds", f"{example}_FT812_{eve2}.txt"))
                    print("Writing gild file:", gild)

                    if (res.returncode != 0):
                        raise Exception(f"Boottester failed {res}.")
                    try:
                        with open(gild, "wb") as gildf:
                            gildf.write(res.stdout)
                    except:
                        raise Exception("Failed to write to gild file.")

                    # EVE1

                    try:
                        cmd = ['cmake', '--fresh', '-B build', '-S .', '-DEVE_DEVICE=EVE_FT800', f"-DEVE_DISPLAY_RES=EVE_{eve1}", f"-DASSETS={assets}", f"-DEXAMPLE={example}"]
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("CMake failed to configure.")
                    
                    if (res.returncode != 0):
                        raise Exception(f"CMake configure error {res}.")

                    try:
                        cmd = ['cmake', '--build', 'build']
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("CMake failed to build.")
                    
                    if (res.returncode != 0):
                        raise Exception(f"CMake build error {res}.")

                    try:
                        cmd = [target]
                        print("Running:", ' '.join(cmd))
                        res = subprocess.run(cmd, stdout=subprocess.PIPE)
                    except:
                        raise Exception("Boottester failed to run.")

                    gild = os.path.normpath(os.path.join("gilds", f"{example}_FT800_{eve1}.txt"))
                    print("Writing gild file:", gild)

                    if (res.returncode != 0):
                        raise Exception(f"Boottester failed {res}.")
                    try:
                        with open(gild, "wb") as gildf:
                            gildf.write(res.stdout)
                    except:
                        raise Exception("Failed to write to gild file.")


                    print("Done")

except:
    raise Exception("The workflow file cannot be found")
