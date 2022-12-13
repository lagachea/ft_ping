#!/bin/python3
import subprocess

hostnames = ["google.com"]
timeout = 3
to_str = str(timeout) + "s"
original = "ping"
to_test = "./ft_ping"

def printCmd(args: list):
    print(f"Launching with following arguments\n{args}")

def getArgs(name: str) -> list:
    args = [
        "timeout",
        "-s",
        "SIGINT",
        to_str,
        original,
        name
    ]
    return args

def getTArgs(name: str) -> list:
    args = getArgs(name)
    last = args.pop()
    args.append("sudo")
    args.append(to_test)
    args.append(last)
    return args

for hname in hostnames:
    # do the original
    args = getArgs(hname)
    printCmd(args)
    with subprocess.Popen(args, stdout=subprocess.PIPE) as ping:
        ping.communicate()
        # what to pipe to to be able to compare output with 

    # do the same with to test ping
    targs = getTArgs(hname)
    printCmd(targs)
    with subprocess.Popen(args) as test:
        test.communicate()
