#! /usr/bin/env python

import socket

UDP_IP = "localhost"
UDP_PORT = 4960

MESSAGE1 = "ID;"
MESSAGE2 = "TEST;CMD=START;DURATION=60;RATE=1000;"
MESSAGE3 = "STOP;"

messages = [MESSAGE1, MESSAGE2, MESSAGE3]

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # create UDP socket object

for message in messages:
  sock.sendto(message.encode("iso-8859-1"), (UDP_IP, UDP_PORT)) 

# print("Antonio Redekop")