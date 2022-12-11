#!/bin/python3

from __future__ import annotations
import subprocess, shlex

def getcmd(cmd: str) -> list[str]:
    return shlex.split(cmd)

# (flush start) firewall
flush_cmd = "iptables -F"
subprocess.run(getcmd(flush_cmd))
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
    print('\nSetting outgoing rules')
    subprocess.run(getcmd(outgoing))
elif target == 2:
    print('\nSetting incoming rules')
    subprocess.run(getcmd(incoming))
elif target == 3:
    print('\nNo rules added')
    exit(0)

list_cmd = getcmd('iptables -S')
with subprocess.Popen(list_cmd, stdout=subprocess.PIPE) as list_rules:

    grep_cmd = getcmd('grep DROP')
    with subprocess.Popen(grep_cmd, stdin=list_rules.stdout) as grep_rules:
        grep_rules.communicate()
