import socket

s = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)
# HDRINCL in IP protocol means I don't need to set and send IP header
# kernel will do it for me
s.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 0)

icmp_header = b'\x08\x00\xe5\xca' # Type of message, Code | Checksum
icmp_header += b'\x12\x34\x00\x01' # Identifier | Sequence Number

addr = '8.8.8.8'
addr = 'google.com'
addr = '42.fr'
s.sendto(icmp_header, (addr, 0))
