#! /usr/bin/env python

import socket

UDP_IP = 'localhost'
UDP_PORT = 4955
STOP_MESSAGE = "STOP;"  # tell device to stop listening
# MESSAGE = "Hello, we've been trying to reach you with regard to your vehicle's extended warranty"

# getaddrinfo returns list of (5) tuples with structure 
# [family, type, proto, canonname, sockaddr]
# addrinfo = socket.getaddrinfo(UDP_IP, UDP_PORT)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # create UDP socket object

sock.sendto(STOP_MESSAGE.encode("iso-8859-1"), (UDP_IP, UDP_PORT)) 

# print("Antonio Redekop")