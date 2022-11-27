import socket
import os

pid = os.getpid()
uid = os.getuid()
if uid != 0:
    print('Cannot use raw socket when not root')
    exit(1);
# addrs = ["9.9.9.9","www.42.fr","google.com"]
# addr_infos = [socket.getaddrinfo(addr, "") for addr in addrs]
# print(addr_infos)

s = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_TCP)
s.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)

ip_header = b'\x45\x00\x00\x1c' # Version, IHL, Type of Service | Total Length
ip_header += b'\xab\xcd\x00\x00' # Identification | Flags, Fragment Offset
ip_header += b'\x40\x01\x6b\xd8' # TTL, Protocol | Header Checksum
ip_header += b'\xc0\xa8\x92\x83' # Source Address
ip_header += b'\x08\x08\x08\x08' # Destination Address

icmp_header = b'\x08\x00\xe5\xca' # Type of message, Code | Checksum
icmp_header += b'\x12\x34\x00\x01' # Identifier | Sequence Number

packet = ip_header + icmp_header
s.sendto(packet, ('8.8.8.8', 0))
