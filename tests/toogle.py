#!/bin/python3

import subprocess, os

target = False

while not target:
    target_nbr = input("Enter 1 to toggle outgoing packets\nEnter 2 to toggle incoming packets\n")
    try:
        target = int(target_nbr)
        if not 1 <= target <= 2:
            target = False
            print("Please enter a correct value")
    except ValueError:
        print("Erro")

source, destination = "/etc/ufw/before.rules", "/etc/ufw/before.rules.01"

if os.path.exists(destination) == False:
    subprocess.run(["cp", source, destination])

#test on/off grep icmp rule
def toggleLine(line):
    if line[0] == "#":
        line = line[1:]
    else :
        line = "#" + line
    return line

echoRequest = "-A ufw-before-output -p icmp -m icmp --icmp-type 8 -j DROP"
echoReply = "-A ufw-before-input -p icmp -m icmp --icmp-type 0 -j DROP"

target_line = ""

if target == 1:
    target_line = echoRequest
elif target == 2:
    target_line = echoReply


idx = 0
index = 0
newline = target_line
with open("/etc/ufw/before.rules",'r') as r:
    lines = [line for line in r]
    for line in lines:
        idx += 1
        if line.__contains__(target_line):
            index = idx - 1
            newline = toggleLine(line)
lines[index] = newline
r.close()

with open("/etc/ufw/before.rules",'w') as w:
    w.writelines(lines)
    w.close()

subprocess.run(["ufw", "reload"])
