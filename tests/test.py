#!/bin/python3
import subprocess, shlex, signal

def getcmd(cmd: str) -> list[str]:
    return shlex.split(cmd)
	
original = "./ping"
to_test = "./ft_ping"

out_orig = []
out_test = []

hostnames = ["255.255.255.255", "0.0.0.0", "127.1.1.1", "localhost", "google.com", "9.9.9.9", "www.42.fr", "192.168.1.1" ]

maxtimeout = 2.6789


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
    file.write(buffer + "\n")
    file.flush()

def getOutname(args: list) -> str:
    bin, target = args[0], args[-1]
    res = ""
    if bin == original:
        res = "tests/orig/{0}.txt".format(target)
        out_orig.append(res)
    elif bin == to_test:
        res = "tests/test/{0}.txt".format(target)
        out_test.append(res)
    return res


def runArgs(args: list, timeout=maxtimeout) -> int:
    outfile = getOutname(args)
    with open(outfile,"w+") as sout, subprocess.Popen(args, stdout=sout, stderr=sout) as proc:
        print(f"[[-x ]] |{args}| Process launched")
        try:
            proc.communicate(timeout=timeout)
        except subprocess.TimeoutExpired:
            writeToFile(sout, "^C SIGINT")
            proc.send_signal(signal.SIGINT)
            print(f"[[--x]] |{args}| Process Finished")
        return proc.returncode


for hname in hostnames:

    print('testing {0}'.format(hname))

    # do the tested
    test_result = runArgs(getTArgs(hname))

    # do the original
    orig_result = runArgs(getArgs(hname))


    orig = out_orig[-1]
    test = out_test[-1]
    diff = "tests/diff/{}".format(test.split('/')[-1])

    fargs = getcmd("sdiff -t {} {}".format(orig, test))
    pargs = getcmd("sdiff -s {} {}".format(orig, test))

    with open(diff, "w+") as filediff, subprocess.Popen(fargs, stdout=filediff, stderr=filediff) as fproc, subprocess.Popen(pargs) as pproc:
        print('{0} result\n\n'.format(hname))
        pproc.communicate()
        print("\n\n\n")
        fproc.communicate()
