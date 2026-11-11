
# Typically run with "python update.py"
# This will update the gild files for the boottester
import os
import re
import yaml
import subprocess

# Source directories for source files to be copied to sketch
workflow = os.path.normpath("../../.github/workflows/api_test.yml")
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
        name = action.get("name")
        config = action.get("config")
    except:
        raise Exception("The workflow actions are invalid")

    print("Updating gild for", name)

    gild = action.get("gild")
    steps = build.get("steps")

    config_step = None
    build_step = None
    run_step = None

    for step in steps:
        step_name = step.get("name")
        if step_name:
            if step_name.startswith("Configure CMake"):
                config_step = step
            if step_name.startswith("Build"):
                build_step = step
            if step_name.startswith("Run"):
                run_step = step

    def yamlrep(s):
        try:
            s = re.sub(r"\${{matrix.eve.config}}", config, s)
            s = re.sub(r"\${{matrix.eve.gild}}", gild, s)
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

    runstep(config_step, "--fresh")
    sbuild = runstep(build_step, "--clean-first")
    target = gettarget(sbuild)
    assert(target)
    expected = runstep(run_step)

    # Fix windows line endings
    expected = re.sub("\r\n", "\n", expected)

    print("Updating gild file:", os.path.join("gilds", gild))
    
    with open(os.path.join("gilds", gild), "w") as g:
        g.write(expected)
    
    print("Done")

