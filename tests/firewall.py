#!/bin/python3

import subprocess, shlex

# (flush start) firewall
firewall_cmd = 'iptables'
subprocess.run([firewall_cmd, "-F"])
print('FLUSHED FIREWAL RULES')

target = False
while not target:
    target_nbr = input("1 to toggle outgoing packets\n2 to toggle incoming packets\n3 to restore\n")
    try:
        target = int(target_nbr)
        if not 1 <= target <= 3:
            target = False
            print("Please enter a correct value")
    except ValueError:
        print("Erro")

incoming = "iptables -I INPUT -p icmp -j DROP"
outgoing = "iptables -I OUTPUT -p icmp -j DROP"

if target == 1:
    # print('output')
    subprocess.run(shlex.split(outgoing))
elif target == 2:
    # print('input')
    subprocess.run(shlex.split(incoming))

subprocess.run([firewall_cmd, "-S"])
