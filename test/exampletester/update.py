
# Typically run with "python update.py"
# This will update the gild files for the exampletester
import os
import re
import yaml
import subprocess

# Source directories for source files to be copied to sketch
workflow = os.path.normpath("../../.github/workflows/example_test.yml")
print("Workflow file: ", workflow)

# Load YAML file
try:
    with open(workflow, "r") as workflowf:
        flow = yaml.load(workflowf, Loader=yaml.SafeLoader)
except:
    raise Exception("The workflow file cannot be found")

# Decode YAML file
try:
    env = flow.get("env")
    build_type = env.get("BUILD_TYPE")
    build_dir = env.get("BUILD_DIR")
    target = env.get("TARGET")

    jobs = flow.get("jobs")
    build = jobs.get("build")
    strategy = build.get("strategy")
    matrix = strategy.get("matrix")
    actions = matrix.get("eve")
except:
    raise Exception("The workflow file is invalid")

for action in actions:
    try:
        name = action.get("EXAMPLE")
        gilded = action.get("GILDED")
        assets = str(action.get("ASSETS"))
        eve1 = action.get("EVE1")
        eve2 = action.get("EVE2")
        eve3 = action.get("EVE3")
        eve4 = action.get("EVE4")
        eve5 = action.get("EVE5")
    except:
        raise Exception("The workflow actions are invalid")
    config = ""

    print("Updating all gilds for", name)

    steps = build.get("steps")

    config_step = None
    build_step = None
    run_step = None
    dsteps = []

    for d in ["EVE1", "EVE2", "EVE3", "EVE4", "EVE5"]:

        if d == "EVE1" and eve1 == False: continue
        if d == "EVE2" and eve2 == False: continue
        if d == "EVE3" and eve3 == False: continue
        if d == "EVE4" and eve4 == False: continue
        if d == "EVE5" and eve5 == False: continue

        for step in steps:
            step_name = step.get("name")
            if step_name:
                if step_name.endswith(d):
                    if step_name.startswith("Configure CMake"):
                        config_step = step
                    if step_name.startswith("Build"):
                        build_step = step
                    if step_name.startswith("Run"):
                        run_step = step
        if config_step and build_step and run_step:
            dsteps.append({"device": d, "config": config_step, "build": build_step, "run": run_step,})
        else:
            raise Exception("The workflow actions are incomplete")

    def yamlrep(s):
        try:
            s = re.sub(r"\${{matrix.eve.config}}", config, s)
            #s = re.sub(r"\${{matrix.eve.gild}}", gild, s)
            s = re.sub(r"\${{matrix.eve.EXAMPLE}}", name, s)
            if eve1: s = re.sub(r"\${{matrix.eve.EVE1}}", eve1, s)
            if eve2: s = re.sub(r"\${{matrix.eve.EVE2}}", eve2, s)
            if eve3: s = re.sub(r"\${{matrix.eve.EVE3}}", eve3, s)
            if eve4: s = re.sub(r"\${{matrix.eve.EVE4}}", eve4, s)
            if eve5: s = re.sub(r"\${{matrix.eve.EVE5}}", eve5, s)
            s = re.sub(r"\${{matrix.eve.ASSETS}}", assets, s)
            s = re.sub(r"\${{env.BUILD_TYPE}}", build_type, s)
            s = re.sub(r"\${{env.BUILD_DIR}}", build_dir, s)
            s = re.sub(r"\${{env.TARGET}}", target, s)
            s = re.sub(r"\${{github.workspace}}", os.curdir, s)
        except:
            raise Exception(f"YAML file replacement failed.")
        return s.split()

    def runstep(step, add=None):
        try:
            cmd = yamlrep(step.get("run"))
            if add:
                cmd.append(add)
            print("Running:", step.get("name"))
            # Windows from unux paths
            ncmd = []
            for c in cmd:
                if c == ">": break
                ncmd.append(os.path.normpath(c))
            print(' '.join(ncmd))
            res = subprocess.run(ncmd, stdout=subprocess.PIPE)
        except:
            raise Exception(f"{step.get('name')} failed.")
        if (res.returncode != 0):
            raise Exception(f"{step.get('name')} error {res.returncode}.")
        return res.stdout.decode('utf-8')

    def gettarget(s):
        target = None
        slines = s.splitlines()
        for t in slines:
            t = t.strip()
            if t.startswith("TARGET: "):
                target = t.split(" ", 1)[1]
        if target:
            target = os.path.relpath(target, os.path.abspath(build_dir))
        return target

    def get_gild(s):
        gild = None
        cmd = yamlrep(s)
        flag = 0
        for c in cmd:
            if flag: 
                gild = os.path.normpath(c)
                break
            if c == ">": 
                flag = 1
        if gild:
            gild = os.path.relpath(gild, os.path.abspath(build_dir))
        return gild

    for d in dsteps:

        print("Updating gilds for", name, d.get("device"))

        config_step = d.get("config")
        build_step = d.get("build")
        run_step = d.get("run")

        runstep(config_step, "--fresh")
        sbuild = runstep(build_step, "--clean-first")
        target = gettarget(sbuild)
        assert(target)
        expected = runstep(run_step)

        if gilded:
            gild = get_gild(run_step.get("run"))
            print(gild)

            # Fix windows line endings
            expected = re.sub("\r\n", "\n", expected)

            print("Updating gild file:", os.path.join("gilds", gild))
            
            with open(os.path.join("gilds", gild), "w") as g:
                g.write(expected)
        
        print("Done")
