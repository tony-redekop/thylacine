import random
from PySide6 import QtCore, QtWidgets, QtGui

#print(sys.argv[0])

class MainWindow(QtWidgets.QWidget):  # extend QWidget class
  def __init__(self):
    super().__init__()              # invoke parent constructor

    self.hello = ["Hello Welt", "Hello World!", "Hola Mundo"]

    self.button = QtWidgets.QPushButton("Click Me!")
    self.text = QtWidgets.QLabel("Hello World",
      alignment = QtCore.Qt.AlignCenter)
    
    self.layout = QtWidgets.QVBoxLayout(self)
    self.layout.addWidget(self.text)
    self.layout.addWidget(self.button)

    # we connect a signal to a slot
    self.button.clicked.connect(self.magic)
  
  @QtCore.Slot()
  def magic(self):
    self.text.setText(random.choice(self.hello))