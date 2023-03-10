import os
import time
import queue
import random
from PySide6 import QtCore, QtWidgets
from modules.consolewidget import ConsoleWidget
from modules.constants import UDP_IP, UDP_PORT, UDP_TIMEOUT, MESSAGES

class MainWindow(QtWidgets.QWidget):  # extend QWidget class
  def __init__(self, socket, recv_queue):
    super().__init__()            
    self.socket = socket
    # Define thread-safe queues allowing communication between client and MainWindow
    self.recv_queue = recv_queue
    self.send_queue = queue.Queue()
    # Scaffold our main window and create widgets (including custom ConsoleWidget)
    self.build_ui()
    # Connect signals to slots 
    self.send_button.clicked.connect(self.send_message)
    self.connect_button.clicked.connect(self.connect_to_host)
    # Write default console message
    self.consolewidget.write("(client) $ Welcome to thylacine client demo!\n")

  @QtCore.Slot()
  def connect_to_host(self):
    # Start server
    self.consolewidget.write("(client) $ Starting server...\n")
    pid1 = os.fork()
    if pid1 == 0: # child process
      os.execv("../server/build/src/demo", ["demo", str(UDP_PORT), str(UDP_TIMEOUT)])
    else: 
      time.sleep(1)                     # allow time for server startup
      self.send_message(MESSAGES[0])    # send discovery message
      response = self.recv_queue.get()  # wait for response (blocking)

      # Write to our ConsoleWidget
      self.consolewidget.write(response) 
      ready_msg = "\n(client) $ Ready for UDP communication\n"
      default_msg = "\n***\n" + "Default messages:\n"
      for index, message in list(enumerate(MESSAGES)):
        default_msg = default_msg + str(index) + ") " + message + "\n" 
      default_msg = default_msg + "***\n"
      self.consolewidget.write(default_msg)

  @QtCore.Slot()
  def send_message(self, message):
    self.send_queue.put(message)

  def build_ui(self):
    self.setWindowTitle("UDP Client")

    # Define QLabels 
    self.label_port = QtWidgets.QLabel("Port:")
    self.label_hostname = QtWidgets.QLabel("Host:")

    # Define QButtons
    self.send_button = QtWidgets.QPushButton("SEND")
    self.connect_button = QtWidgets.QPushButton("START SERVER")
    self.connect_button.setFixedHeight(40)
    self.send_button.setFixedHeight(40)

    # Define QGroupBoxes 
    self.groupbox_tests = QtWidgets.QGroupBox("Tests")
    self.groupbox_sessions = QtWidgets.QGroupBox("Sessions")
    self.groupbox_active = QtWidgets.QGroupBox("Active session")
    self.groupbox_active.setFixedHeight(400)
    self.groupbox_tests.setFixedWidth(500)

    # Define QLineEdits 
    self.lineedit_port = QtWidgets.QLineEdit()
    self.lineedit_message = QtWidgets.QLineEdit()
    self.lineedit_hostname = QtWidgets.QLineEdit()
    self.lineedit_hostname.setFixedHeight(40)
    self.lineedit_port.setFixedHeight(40)
    self.lineedit_message.setFixedHeight(40)

    # Define QListWidgets 
    self.listwidget_tests = QtWidgets.QListWidget(self)
    self.listwidget_sessions = QtWidgets.QListWidget(self)

    # Define custom Console widget
    self.consolewidget = ConsoleWidget()
    self.consolewidget.setFixedWidth(600)

    # Define main layouts
    self.layout_main = QtWidgets.QHBoxLayout(self)
    self.layout_mainleft = QtWidgets.QVBoxLayout()
    self.layout_mainright = QtWidgets.QVBoxLayout()
    
    # Define nested layouts
    self.layout_serverinfo = QtWidgets.QVBoxLayout()      
    self.layout_serverinfo_row1 = QtWidgets.QHBoxLayout()
    self.layout_serverinfo_row2 = QtWidgets.QHBoxLayout()
    self.layout_sendmessage = QtWidgets.QHBoxLayout()
    self.layout_sessions = QtWidgets.QVBoxLayout()
    self.layout_active = QtWidgets.QVBoxLayout()
    self.layout_tests = QtWidgets.QVBoxLayout()

    # Set group box layouts 
    self.groupbox_active.setLayout(self.layout_active)
    self.groupbox_sessions.setLayout(self.layout_sessions)
    self.groupbox_tests.setLayout(self.layout_tests)
    
    # Configure main layouts
    self.layout_main.addLayout(self.layout_mainleft)
    self.layout_main.addSpacing(25)
    self.layout_main.addLayout(self.layout_mainright)
    self.layout_main.setContentsMargins(25, 25, 25, 25)
    self.layout_mainleft.addLayout(self.layout_serverinfo)
    self.layout_mainleft.addSpacing(10)
    self.layout_mainleft.addWidget(self.connect_button)
    self.layout_mainleft.addSpacing(10)
    self.layout_mainleft.addWidget(self.groupbox_active)
    self.layout_mainright.addWidget(self.groupbox_sessions)
    self.layout_mainright.addSpacing(10)
    self.layout_mainright.addWidget(self.groupbox_tests)

    # Configure nested layouts 
    self.layout_serverinfo.addLayout(self.layout_serverinfo_row1)
    self.layout_serverinfo.addSpacing(10)
    self.layout_serverinfo.addLayout(self.layout_serverinfo_row2)
    self.layout_serverinfo_row1.addWidget(self.label_hostname)
    self.layout_serverinfo_row1.addWidget(self.lineedit_hostname)
    self.layout_serverinfo_row2.addWidget(self.label_port)
    self.layout_serverinfo_row2.addWidget(self.lineedit_port)
    self.layout_active.addWidget(self.consolewidget)
    self.layout_active.addSpacing(10)
    self.layout_active.addLayout(self.layout_sendmessage)
    self.layout_sessions.addWidget(self.listwidget_sessions)
    self.layout_tests.addWidget(self.listwidget_tests)
    self.layout_sendmessage.addWidget(self.lineedit_message)
    self.layout_sendmessage.addWidget(self.send_button)