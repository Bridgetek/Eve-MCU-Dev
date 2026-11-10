
# Typically run with "python update.py"
# This will update the gild files for the boottester
import os
import sys
import yaml
import subprocess

# Source directories for source files to be copied to sketch
workflow = os.path.normpath("../../.github/workflows/api_test.yml")
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
                name = action.get("name")
                config = action.get("config").split()
                gild = os.path.normpath(os.path.join("gilds", action.get("gild")))

                print("Updating gild for", name)
                
                try:
                    cmd = ['cmake', '--fresh', '-B build', '-S .']
                    for c in config: cmd.append(c)
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
