# This is a program for testing Qt GUI and PySide

import sys

# Import Qt GUI component
from PyQt4.QtGui import *

# Import GUI File
from untitled import Ui_Dialog

# Self Function
def PrintHello():
    print("Hello")

# Make main window class
class QDialog(QDialog,Ui_Dialog):
    def __init__(self, parent=None):
        super(QDialog,self).__init__(parent)
        self.setupUi(self)
        # Connect button click event to PrintHello function
        # self.pushButton.clicked.connect(PrintHello)

# End of main window class


# Main Function
if __name__=='__main__':
    Program = QApplication(sys.argv)
    Dialog=QDialog()
    Dialog.show()
    Program.exec_()