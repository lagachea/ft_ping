#!/bin/python3

from __future__ import annotations
import subprocess, shlex

def getcmd(cmd: str) -> list[str]:
    return shlex.split(cmd)

def restore():
    # (flush start) firewall
    flush_cmd = "iptables -F"
    subprocess.run(getcmd(flush_cmd))
    print('Restored firewall rules')

def incoming():
    incoming = "iptables -I INPUT -p icmp -j DROP"
    print('\nSetting incoming rules')
    subprocess.run(getcmd(incoming))


def outgoing():
    outgoing = "iptables -I OUTPUT -p icmp -j DROP"
    print('\nSetting outgoing rules')
    subprocess.run(getcmd(outgoing))

def nothing():
    print('\nNo rules added')

cases = {
    1: outgoing,
    2: incoming,
    3: nothing,
}

def switch():
    to_run = cases.get(target)
    if to_run is not None:
        to_run()
    return

restore()

target = False
while not target:
    target_nbr = input("1 to stop outgoing packets\n2 to stop incoming packets\n3 to stop no packets\n")
    try:
        target = int(target_nbr)
        if not 1 <= target <= 3:
            target = False
            print("Please enter a correct value")
    except ValueError:
        print("Erro")

switch()

list_cmd = getcmd('iptables -S')
with subprocess.Popen(list_cmd, stdout=subprocess.PIPE) as list_rules:

    grep_cmd = getcmd('grep DROP')
    with subprocess.Popen(grep_cmd, stdin=list_rules.stdout) as grep_rules:
        grep_rules.communicate()

exit(0)
