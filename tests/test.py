#!/bin/python3
import subprocess

	
hostnames = ["127.0.0.1", "google.com", "9.9.9.9", "www.42.fr", "192.168.1.1" ]
timeout = 2.6789
to_str = str(timeout) + "s"
original = "./ping"
to_test = "./ft_ping"

def printCmd(args: list):
    print(f"{args}")
            #f"Launching with following arguments\n

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
    args.pop()
    # args.append("sudo")
    args.append(to_test)
    args.append(last)
    return args

for hname in hostnames:
    # do the original
    args = getArgs(hname)
    printCmd(args)
    print("[[===================")
    with subprocess.Popen(args) as ping:
        ping.communicate()
    print("                     ===================]]")
        # what to pipe to to be able to compare output with 
    # do the same with to test ping
    targs = getTArgs(hname)
    printCmd(targs)
    print("[[===================")
    with subprocess.Popen(targs) as test:
        test.communicate()
    print("                     ===================]]")
