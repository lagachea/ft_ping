#!/bin/python3
import subprocess
import signal

	
original = "./ping"
to_test = "./ft_ping"

out_orig = []
out_test = []

hostnames = ["255.255.255.255", "0.0.0.0", "127.1.1.1", "localhost", "google.com", "9.9.9.9", "www.42.fr", "192.168.1.1" ]

maxtimeout = 2.6789

def printCmd(args: list):
    print(f"{args}")
            #f"Launching with following arguments\n

def getArgs(name: str) -> list:
    args = [ original, name ]
    # print("[[===================")
    # printCmd(args)
    # print("                     ===================]]")
    return args

def getTArgs(name: str) -> list:
    args = [ to_test, name ]
    # print("[[===================")
    # printCmd(args)
    # print("                     ===================]]")
    return args

def writeToFile(file, buffer: str):
    file.write(buffer)
    file.flush()

def getOutname(args: list) -> str:
    bin, host = args[0], args[1]
    if bin == original:
        res = "tests/orig/{0}.txt".format(host)
        out_orig.append(res)
    else:
        res = "tests/test/{0}.txt".format(host)
        out_test.append(res)
    return res


def runArgs(args: list, timeout=maxtimeout) -> int:
    outfile = getOutname(args)
    with open(outfile,"w+") as sout:
        with subprocess.Popen(args, stdout=sout, stderr=sout) as proc:
            writeToFile(sout, "[[-x ]] Process launched\n")
            try:
                proc.communicate(timeout=timeout)
            except subprocess.TimeoutExpired:
                writeToFile(sout, "^C SIGINT\n")
                proc.send_signal(signal.SIGINT)
                proc.communicate()
                writeToFile(sout, "[[--x]] Process Finished\n")
            sout.close()
            return proc.returncode


for hname in hostnames:
    print('\ntesting {0}'.format(hname))
    # do the original
    orig_result = runArgs(getArgs(hname))

    # do the tested
    test_result = runArgs(getTArgs(hname))

    print('\n{0} tested'.format(hname))
