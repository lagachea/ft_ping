#!/bin/python3

from __future__ import annotations
import subprocess, shlex

def getcmd(cmd: str) -> list[str]:
    return shlex.split(cmd)

def restore():
    # (flush start) firewall
    flush_cmd = "iptables -F -t nat"
    subprocess.run(getcmd(flush_cmd))
    flush_cmd = "iptables -F -t filter"
    subprocess.run(getcmd(flush_cmd))
    print('Restored firewall rules')

def incoming():
    print('\nSetting incoming rules')
    incoming = "iptables -A INPUT -p icmp -j DROP"
    subprocess.run(getcmd(incoming))
    incoming = "iptables -A INPUT -i lo -j ACCEPT"
    subprocess.run(getcmd(incoming))
    incoming = "iptables -A OUTPUT -o lo -j ACCEPT"
    subprocess.run(getcmd(incoming))


def outgoing():
    outgoing = "iptables -A OUTPUT -p icmp -j DROP"
    print('\nSetting outgoing rules')
    subprocess.run(getcmd(outgoing))

def nothing():
    print('\nNo rules added')

def rerouting():
    rerouting = "iptables -t nat -A OUTPUT -p icmp -j DNAT --to-destination 127.0.0.1"
    output = "iptables -A INPUT -p icmp -j DROP"
    print('\nSetting rerouting rules')
    subprocess.run(getcmd(rerouting))
    subprocess.run(getcmd(output))

cases = {
    1: outgoing,
    2: incoming,
    3: rerouting,
    4: nothing,
}

case_string = """
1 to stop outgoing packets
2 to stop incoming packets
3 to reroute outgoing packets to localhost (to user with server.py)
4 to stop no packets
"""
cases_len = len(cases)

def switch():
    to_run = cases.get(target)
    if to_run is not None:
        to_run()
    return

restore()

target = False
while not target:
    target_nbr = input(case_string)
    try:
        target = int(target_nbr)
        if not 1 <= target <= cases_len:
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
