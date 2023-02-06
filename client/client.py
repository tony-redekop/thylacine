#! /usr/bin/env python

import socket
import time
import sys
import os

UDP_IP = "localhost"
UDP_PORT = 9000
UDP_TIMEOUT = 0

# Set up demo
messages = []
messages.append("ID;")
messages.append("TEST;CMD=START;DURATION=60;RATE=1000;")
messages.append("TEST;CMD=STOP;DURATION=60;RATE=1000;")  # refactor to remove DURATION and RATE params
messages.append("STOP;")

print("Welcome to thylacine client demo! ")

pid = os.fork()
if pid == 0: # child process
  print("Status: starting server (I/O device)")
  os.execv("../server/build/src/demo", ["demo", str(UDP_PORT), str(UDP_TIMEOUT)])
else: 
  time.sleep(1) # allow for server startup (refactor this later)
  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # create UDP socket object
  print("Status: ready for UDP communication with IP:", UDP_IP, "PORT:", UDP_PORT)

print("***")
print("Default messages:")
for index, message in list(enumerate(messages)):
  print("  " + str(index) + ")", message)
print("  " + str(index+1) + ")", "<CUSTOM>")

while (1):
  selection = int(input("Enter selection: "))
  message = messages[selection]

  # Send
  sock.sendto(message.encode("iso-8859-1"), (UDP_IP, UDP_PORT))

  # Recieve (blocking)
  data = sock.recvfrom(UDP_PORT)
  print("client: Response recieved from", data[1], ":", \
    str(data[0], encoding="iso-8859-1"))

  # Exit condition
  if selection == 3:
    break

# print("Antonio Redekop")