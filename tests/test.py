#!/bin/python3
import shlex, subprocess
command_line = input()

args = shlex.split(command_line)
print(args)

p = subprocess.Popen(args) # Success!

# subprocess.run(["ls", "-l"])  # doesn't capture output
# subprocess.run("exit 1", shell=True, check=True)
# subprocess.run(["ls", "-l", "/dev/null"], capture_output=True)
