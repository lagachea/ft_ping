import netfilterqueue
import sys

def process(pkt):
    data = pkt.get_payload()
    print(pkt, data)
    # pkt.accept()

nfqueue = netfilterqueue.NetfilterQueue()
nfqueue.bind(1, process)

try:
    nfqueue.run()
except:
    nfqueue.unbind()
    sys.exit(1)

