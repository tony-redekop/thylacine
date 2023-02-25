#! /usr/bin/env python

import sys
import queue
import socket
import threading
from PySide6 import QtWidgets, QtCore
from modules.mainwindow import MainWindow 
from modules.constants import UDP_IP, UDP_PORT, UDP_TIMEOUT

def client_logic(sock, send_queue, recv_queue):
  while True:
    message = send_queue.get()  # blocking
    # Send message to server
    sock.sendto(message.encode("iso-8859-1"), (UDP_IP, UDP_PORT))
    # Start new thread to handle recieve loop 
    recieve_loop_thread = threading.Thread(target=recieve_loop, args=(sock, recv_queue))
    recieve_loop_thread.start()

def recieve_loop(sock, recv_queue):
  while True:
    data = sock.recvfrom(UDP_PORT)  # blocking operation
    message = "(client) $ Response recieved from" + str(data[1]) + ":\n" + \
      "(server) $ " + str(data[0], encoding="iso-8859-1")
    recv_queue.put(message)
    if str(data[0], encoding="iso-8859-1") == "TEST;RESULT=STARTED;":
      continue
    else:
      break

# Create UDP socket object
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Create recieve queue
recv_queue = queue.Queue()

# Create main window
app = QtWidgets.QApplication([])
mainwindow = MainWindow(sock, recv_queue)

# Create and start thread to handle client message loop 
client_thread = threading.Thread(target=client_logic, args=(sock, mainwindow.send_queue, recv_queue))
client_thread.start()

# Run GUI on main thread
mainwindow.show()
sys.exit(app.exec())