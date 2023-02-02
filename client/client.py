#! /usr/bin/env python

import socket
import time
import sys
import os

UDP_IP = "localhost"
UDP_PORT = 4960
UDP_TIMEOUT = 0

MESSAGE1 = "ID;"
MESSAGE2 = "TEST;CMD=START;DURATION=60;RATE=1000;"
MESSAGE3 = "STOP;"

pid = os.fork()

if pid == 0: # child process
  os.execv("../server/build/src/demo", ["demo", str(UDP_PORT), str(UDP_TIMEOUT)])
else: 
  time.sleep(2) # allow for device startup (refactor this later) 

messages = [MESSAGE1, MESSAGE2, MESSAGE3]

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # create UDP socket object

for message in messages:
  print("***")
  print("Message sent: ", message)
  
  if message == "STOP;":
    break

  print("Waiting for device response...")

  sock.sendto(message.encode("iso-8859-1"), (UDP_IP, UDP_PORT)) 
  data = sock.recvfrom(UDP_PORT)

  print("Response recieved from", data[1], ":")
  print(str(data[0], encoding="iso-8859-1"))

  time.sleep(2)

# print("Antonio Redekop")