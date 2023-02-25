from PySide6 import QtWidgets, QtCore, QtGui

class ConsoleWidget(QtWidgets.QTextEdit):
  def __init__(self, parent=None):
    super().__init__(parent)
    self.setReadOnly(True)
    self.setLineWrapMode(QtWidgets.QTextEdit.NoWrap)
    self.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOn)
    self.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
    self.setFont(QtGui.QFont("Consolas", 12))

  def write(self, text):
    self.insertPlainText(text)
    self.ensureCursorVisible()