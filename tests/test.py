import socket as sckt
import os

pid = os.getpid()
uid = os.getuid()
if uid != 0:
    print('Cannot use raw socket when not root')
    exit(1)

family, socktype, protocol = sckt.AF_INET, sckt.SOCK_RAW, sckt.IPPROTO_ICMP
sock = sckt.socket(family, socktype, protocol)
sock.setsockopt(sckt.IPPROTO_IP, sckt.IP_HDRINCL, 1)

def getPacket():
    src_addr = sckt.inet_pton(sckt.AF_INET, "192.168.228.88")
    ip_header = b'\x45\x00\x00\x1c' # Version, IHL, Type of Service | Total Length
    ip_header += b'\xab\xcd\x00\x00' # Identification | Flags, Fragment Offset
    ip_header += b'\x40\x01\x6b\xd8' # TTL, Protocol | Header Checksum
    ip_header += src_addr # Source Address
    ip_header += b'\x08\x08\x08\x08' # Destination Address

    # No need for source address if we set IP_HDRINCL to ...

    icmp_header = b'\x08\x00\xe5\xca' # Type of message, Code | Checksum
    icmp_header += b'\x12\x34\x00\x01' # Identifier | Sequence Number

    packet = ip_header + icmp_header
    return packet
    # return icmp_header


def sendPacket(sock):
    # Given a socket send a new packet
    # This sends our packet to google '8.8.8.8'
    packet = getPacket()
    print("sending packet")
    sock.sendto(packet, ('8.8.8.8', 0))
    print("Packet Sent need to read response now")


sendPacket(sock)
# sendPacket(sock)
# sendPacket(sock)
exit(0)

# To be able to fetch our local address from the socket we need connect to the
# remote address
# sock.connect(('8.8.8.8',0))
# print(sock.getsockname())
