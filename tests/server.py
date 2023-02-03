from binascii import hexlify
from struct import unpack, pack
import socket as skt

def unpack_iphdr(data: bytes):
    ver_hl, services, total_length, identification, flags, ttl, protocol,\
            checksum, saddr, daddr = unpack("! B B H H H B B H I I", data)
    version = ver_hl
    header_length = ver_hl
    return version, header_length, services, total_length, identification, flags, ttl, protocol, checksum, saddr, daddr 

def unpack_icmp(data: bytes):
    icmp_type, code, checksum, identifier, sequence, t_sec, t_usec = unpack("! B B H H H q q", data)
    return icmp_type, code, checksum, identifier, sequence#, t_sec, t_usec


socket = skt.socket(skt.AF_INET, skt.SOCK_RAW, skt.IPPROTO_ICMP)

while True:
    (dataframe, address) = socket.recvfrom(65536)
    datalen = len(dataframe)
    print("found packet with len", datalen)
    if datalen == 84:
        iphdr = unpack_iphdr(dataframe[:20])
        saddr = dataframe[16:20]
        saddr = skt.inet_ntop(skt.AF_INET, saddr)
        daddr = dataframe[12:16]
        daddr = skt.inet_ntop(skt.AF_INET, daddr)
        icmphdr = unpack_icmp(dataframe[20:-40])
        icmpdata = dataframe[-56:]
        if saddr != daddr:
            print("\nNew packet")

            print("FROM {0} TO {1}".format(saddr, daddr))

            print("ICMP HEADER PRE", icmphdr)
            tmp = (8 , icmphdr[1], icmphdr[2] + 256, icmphdr[3], icmphdr[4])
            print("ICMP HEADER POST", tmp)

            print("ICMP DATA", hexlify(icmpdata, " "))

            print("FROM", address)

            data = pack("! B B H H H", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);
            
            print("END\n")
            socket.sendto(b''.join([data,icmpdata]), (saddr, 0))
