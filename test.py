import socket as sckt
import os

getLocalAddress = False
pid = os.getpid()
uid = os.getuid()
family, socktype, protocol = sckt.AF_INET, sckt.SOCK_RAW, sckt.IPPROTO_TCP
if uid != 0:
    getLocalAddress = True
    print('Cannot use raw socket when not root')
    exit(1)

# addrs = ["9.9.9.9","www.42.fr","google.com"]
# addr_infos = [socket.getaddrinfo(addr, "") for addr in addrs]
# print(addr_infos)

sock = sckt.socket(family, socktype, protocol)
print(sock)
sock.setsockopt(sckt.IPPROTO_IP, sckt.IP_HDRINCL, 1)

src_addr = sckt.inet_pton(sckt.AF_INET, "172.20.76.6")
ip_header = b'\x45\x00\x00\x1c' # Version, IHL, Type of Service | Total Length
ip_header += b'\xab\xcd\x00\x00' # Identification | Flags, Fragment Offset
ip_header += b'\x40\x01\x6b\xd8' # TTL, Protocol | Header Checksum
ip_header += src_addr # Source Address
ip_header += b'\x08\x08\x08\x08' # Destination Address

icmp_header = b'\x08\x00\xe5\xca' # Type of message, Code | Checksum
icmp_header += b'\x12\x34\x00\x01' # Identifier | Sequence Number

packet = ip_header + icmp_header

# This sends our packet to google '8.8.8.8'
sock.sendto(packet, ('8.8.8.8', 0))
if getLocalAddress is False:
    # socktype = sckt.SOCK_DGRAM
    exit(0)

# To be able to fetch our local address from the socket we need connect to the
# remote address
sock.connect(('8.8.8.8',0))
print(sock.getsockname())
