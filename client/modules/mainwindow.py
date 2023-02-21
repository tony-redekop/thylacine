import queue
import random
from PySide6 import QtCore, QtWidgets, QtGui
from modules.constants import UDP_IP, UDP_PORT, UDP_TIMEOUT, MESSAGES

class MainWindow(QtWidgets.QWidget):  # extend QWidget class
  def __init__(self):
    super().__init__()            

    self.msg_queue = queue.Queue()

    # self.text = QtWidgets.QLabel(alignment=QtCore.Qt.AlignCenter)
    self.text = QtWidgets.QLabel()
    mystr = "Welcome to thylacine client demo!\n" + \
      "Status: ready for UDP communication with IP: " + str(UDP_IP) + " PORT: " + str(UDP_PORT) + \
      "\n***\n" + \
      "Default messages:\n" 
    for index, message in list(enumerate(MESSAGES)):
      mystr = mystr + str(index) + ") " + message + "\n" 
      # print("  " + str(index+1) + ")", "<CUSTOM>")

    self.text.setText(mystr)

    self.button = QtWidgets.QPushButton("Send message")
    self.layout = QtWidgets.QVBoxLayout(self)
    self.layout.addWidget(self.text)
    self.layout.addWidget(self.button)

    # We connect a signal to a slot
    self.button.clicked.connect(self.send_message)
  
  @QtCore.Slot()
  def send_message(self):
    message = MESSAGES[0]
    self.msg_queue.put(message)