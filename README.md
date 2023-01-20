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
Validate response before print

Write a usage

Handle next packet wait time without alarm?

using state 
    SENDING(sendto)
    RECIEVING(recvmsg)
    WAITING(post print rcv)
    READY(post wait)

move looping inside loop and use if state READY
