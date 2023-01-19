### This repo is a ping in c

The goal here is to recreate the ping command in c using minimal function (see subject pdf)

### The things I already done

DNS query to get ip from hostname
create packet
send packet
recieve response
print RTT and various info
print info on SIGQUIT
quit gracefully on SIGINT

### My next goal is:
-v adds:
    <!-- print info about socket and ai family before sending packets -->
    ident at each response print

-h adds:
    only prints usage and quit gracefully
