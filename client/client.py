#! /usr/bin/env python

import os
import sys
import time
import queue
import socket
import threading
from PySide6 import QtWidgets
from modules.mainwindow import MainWindow 
from modules.constants import UDP_IP, UDP_PORT, UDP_TIMEOUT

def client_logic(sock, msg_queue):
  while True:
    message = msg_queue.get()  # blocking
    # Send message to server
    sock.sendto(message.encode("iso-8859-1"), (UDP_IP, UDP_PORT))
    # Start new thread to handle recieve loop 
    recieve_loop_thread = threading.Thread(target=recieve_loop, args=(sock,))
    recieve_loop_thread.start()

def recieve_loop(sock):
  while True:
    data = sock.recvfrom(UDP_PORT)  # blocking operation
    print("client: Response recieved from", data[1], ":", \
      str(data[0], encoding="iso-8859-1"))
    if str(data[0], encoding="iso-8859-1") == "TEST;RESULT=STARTED;":
      continue
    else:
      break

# Start server
pid1 = os.fork()
if pid1 == 0: # child process
  print("Status: starting server (I/O device)")
  os.execv("../server/build/src/demo", ["demo", str(UDP_PORT), str(UDP_TIMEOUT)])
else: 
  time.sleep(1) # allow some time for server startup

# Create UDP socket object
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # create UDP socket object

# Create main window
app = QtWidgets.QApplication([])
mainwindow = MainWindow()
mainwindow.resize(800, 600)

# Create and start thread to handle client message loop 
client_thread = threading.Thread(target=client_logic, args=(sock, mainwindow.msg_queue))
client_thread.start()

# Run GUI on main thread
mainwindow.show()
sys.exit(app.exec())