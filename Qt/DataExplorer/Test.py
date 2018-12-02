from PyQt4.QtGui import *
from PyQt4.QtCore import *
from win32gui import *

class Test(QLabel):
    
    def __init__(self, parent=None):
        QLabel.__init__(self,parent)
        
        #self.setWindowTitle('Test')
        self.setText('Test')
        
        win0 = FindWindow(None,'Data Explorer')
        print win0
        win1 = self.winId()#FindWindow(None,'Test')
        print win1
        win2 = FindWindow(None,'Waveform Chart.vi')
        print win2
        SetParent(win1,win0)
        #ShowWindow(win1,1)
        SetParent(win2,win1)
        
        self.resize(600, 600)
        self.show()
        

        