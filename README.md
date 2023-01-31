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
now that we have non blocking msg reception we can extends the main loop
to Send / Recieve / Wait blocks

use a bigger icmp packet to add timestamp in packet data

add a server to respond to packet and modify them to test program
