#!/bin/python3

import subprocess

destination, source = "/etc/ufw/before.rules", "./before.rules.01"
subprocess.run(["cp", source, destination])
