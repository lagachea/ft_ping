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
in case of 127.0.0.0 subnet recvmsg read the message we just sent
add a loop waiting for correct response in case of local routing ?
