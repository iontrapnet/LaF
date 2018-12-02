import math,random,time,csv,itertools,colorsys,os,sys
from PyQt4.qcustomplot import *
from PyQt4.QtGui import *
from PyQt4.QtCore import *
sys.path.append(os.path.realpath('..'))
from labview import *
from win32gui import *

def split_seq(iterable, size):
    it = iter(iterable)
    item = list(itertools.islice(it, size))
    while item:
        yield item
        item = list(itertools.islice(it, size))

def reverse_mask(x):
    x = ((x & 0x55555555) << 1) | ((x & 0xAAAAAAAA) >> 1)
    x = ((x & 0x33333333) << 2) | ((x & 0xCCCCCCCC) >> 2)
    x = ((x & 0x0F0F0F0F) << 4) | ((x & 0xF0F0F0F0) >> 4)
    x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8)
    x = ((x & 0x0000FFFF) << 16) | ((x & 0xFFFF0000) >> 16)
    return x
    
ColorValues=[
        "FF0000", "00FF00", "0000FF", "FFFF00", "FF00FF", "00FFFF", "000000",
        "800000", "008000", "000080", "808000", "800080", "008080", "808080",
        "C00000", "00C000", "0000C0", "C0C000", "C000C0", "00C0C0", "C0C0C0",
        "400000", "004000", "000040", "404000", "400040", "004040", "404040",
        "200000", "002000", "000020", "202000", "200020", "002020", "202020",
        "600000", "006000", "000060", "606000", "600060", "006060", "606060",
        "A00000", "00A000", "0000A0", "A0A000", "A000A0", "00A0A0", "A0A0A0",
        "E00000", "00E000", "0000E0", "E0E000", "E000E0", "00E0E0", "E0E0E0"
]

def hex_to_rgb(value):
	value = value.lstrip('#')
	lv = len(value)
	return tuple(int(value[i:i+lv/3], 16) for i in range(0, lv, lv/3))

class Plotter(QCustomPlot):
    CountsFunctions = {'S0':lambda x: sum(x) > 1, 'S1':lambda x: x[0] > 1, 'S2':lambda x: x[1] > 1, 'S3':lambda x: x[2] > 1, 'S4':lambda x: x[3] > 1, 'C0':lambda x: sum(x), 'C1':lambda x: x[0], 'C2':lambda x: x[1], 'C3':lambda x: x[2], 'C4':lambda x: x[3]}
    CountsPlotMode = 1
    TemperaturePlotMode = 2
    
    def __init__(self, parent=None):
        QCustomPlot.__init__(self,parent)
        self.mousePress.connect(self.onMousePress)
        self.mouseWheel.connect(self.onMouseWheel)
        self.mouseDoubleClick.connect(self.onMouseDoubleClick)
        self.setInteractions(QCP.Interactions(QCP.iRangeDrag | QCP.iRangeZoom | QCP.iSelectAxes | QCP.iSelectLegend))
        self.axisRect().setupFullAxesBox()
        self.xAxis2.setTicks(False)
        self.yAxis2.setTicks(False)
        self.CountsFunction = 'S0'
        self.setContextMenuPolicy(Qt.DefaultContextMenu)
    
    def onShowAction(self):
        if self.mode == self.CountsPlotMode:
            vi = LoadVI('XY',8)
            vi.FPWinOpen = True
            win = vi._FPWinOSWindow
            SetWindowText(win,str(self.path))
            SetForegroundWindow(win)
            vi.Call(['X','Y'],self.counts)
        
    def contextMenuEvent(self, event):
        pos = event.pos()
        menu = QMenu()
        if self.mode == self.CountsPlotMode:
            showAction = QAction("Show In LabVIEW",menu,triggered=self.onShowAction)
            menu.addAction(showAction)
        quitAction = menu.addAction("Quit")
        action = menu.exec_(self.mapToGlobal(pos))
        if action == quitAction:
            qApp.quit()
        event.accept()
        
    def onMousePress(self, event):
        if int(self.xAxis.selectedParts()) & QCPAxis.spAxis:
            self.axisRect().setRangeDrag(self.xAxis.orientation())
        elif int(self.yAxis.selectedParts()) & QCPAxis.spAxis:
            self.axisRect().setRangeDrag(self.yAxis.orientation())
        else:
            self.axisRect().setRangeDrag(Qt.Horizontal | Qt.Vertical)
       
    def onMouseWheel(self, event):
        if int(self.xAxis.selectedParts()) & QCPAxis.spAxis:
            self.axisRect().setRangeZoom(self.xAxis.orientation())
        elif int(self.yAxis.selectedParts()) & QCPAxis.spAxis:
            self.axisRect().setRangeZoom(self.yAxis.orientation())
        else:
            self.axisRect().setRangeZoom(Qt.Horizontal | Qt.Vertical)
                
    def onMouseDoubleClick(self):
        self.rescaleAxes()
        self.replot()
        #self.setWindowFlags(Qt.Popup)
        #self.show()            
        
    def CountsPlot(self):
        self.mode = self.CountsPlotMode
        self.clearGraphs()
        data = self.data
        graph = self.addGraph()
        graph.setData(*self.CountsPlotData())
        graph.setLineStyle(QCPGraph.lsLine)
        graph.setScatterStyle(QCPScatterStyle(QCPScatterStyle.ssDisc,5))
        graph.setName(self.CountsFunction)
        x_label = ''
        if 'Parameter' in data:
            if 'Index' in data:
                x_label = 'Index ' + str(int(data['Index'])+1) + ' ' + data['Parameter']
            else:
                x_label = data['Parameter']
        elif 'Chapter' in data:
            x_label = 'Chapter ' + str(int(data['Chapter'])+1) + ' Duration (us)'
        if x_label:
            self.xAxis.setLabel(x_label)
        self.legend.setVisible(True)
        #self.legend.setSelectableParts(QCPLegend.spItems)
        try: self.legendClick.connect(self.CountsPlotLegendClick, Qt.UniqueConnection)
        except TypeError: pass
        try: self.legendDoubleClick.connect(self.CountsPlotLegendDoubleClick, Qt.UniqueConnection)
        except TypeError: pass
        try: self.axisClick.connect(self.CountsPlotAxisClick, Qt.UniqueConnection)
        except TypeError: pass
        #self.xAxis.setRange(x_data[0],x_data[-1])
        #self.yAxis.setRange(0,1)
        self.rescaleAxes()
        self.replot()
        
    def CountsPlotData(self):
        counts = self.data['Counts']
        x_data = []
        y_data = []
        for line in counts.strip().replace('/\r/g','').split('\n'):
            x,y = line.strip().split(',')
            x = float(x.strip().split(' ')[0])
            y = split_seq(map(int,y.strip().split(' ')),4)
            y = [self.CountsFunctions[self.CountsFunction](c) for c in y]
            y = sum(y)/float(len(y))
            x_data.append(x)
            y_data.append(y)
        self.counts = (x_data,y_data)
        return self.counts
    
    def CountsPlotChangeMode(self):
        self.graph(0).setData(*self.CountsPlotData())
        self.graph(0).setName(self.CountsFunction)
        self.graph(0).setPen(QPen(Qt.blue if self.CountsFunction == 'S0'else Qt.red))
        self.rescaleAxes()
        self.replot()
        
    def CountsPlotLegendClick(self):
        self.CountsFunction = 'C0' if self.CountsFunction == 'S0' else 'S0'
        self.CountsPlotChangeMode()
    
    def CountsPlotLegendDoubleClick(self):
        text,ok = QInputDialog.getText(self, 'CountsFunction', 'CountsFunction:', QLineEdit.Normal, self.CountsFunction)
        if ok:
            self.CountsFunction = str(text)
            self.CountsPlotChangeMode()
    
    def CountsPlotAxisClick(self, axis, part, event):
        pos = event.pos()
        if self.xAxis.selectTest(pos,False) >= 0 and part == QCPAxis.spAxisLabel:
            window = QMainWindow(parent=self)
            window.setWindowTitle(self.path)
            window.resize(600,200)
            model = QStandardItemModel()
            view = QTableView(parent=window)
            view.setModel(model)
            data = self.data
            if 'Parameter' in data:
                if 'Index' not in data:
                    return
                parameter = data['Parameter']
                wave = data['Wave'].strip().split('\n')
                row = wave[0].split(',')
                model.setHorizontalHeaderLabels(row)
                index = [int(data['Index']),row.index(parameter)]
                items = []
                for row in wave[1:]:
                    items = [
                        QStandardItem(field)
                        for field in row.split(',')
                    ]
                    model.appendRow(items)
            elif 'Chapter' in data:
                model.setHorizontalHeaderLabels(['Chapter','Duration (us)'])
                index = [int(data['Chapter']),1]
                chapter,sequence = data['Sequence'].split('-\n')
                sequence = sequence.strip().split('\n')
                for row in sequence:
                    fields = row[0:-1].split('(')
                    if ',' in fields[1]:
                        name = fields[0]
                        durations = fields[1].split(',')
                        for i in xrange(len(durations)):
                            items = [QStandardItem(name+'['+str(i)+']'),QStandardItem(durations[i])]
                            model.appendRow(items)
                    else:
                        items = map(QStandardItem,fields)
                        model.appendRow(items)
            view.setCurrentIndex(model.index(*index))
            view.setAlternatingRowColors(True)
            view.horizontalHeader().setResizeMode(QHeaderView.Stretch)
            view.resizeRowsToContents()
            #view.setEditTriggers(QAbstractItemView.NoEditTriggers)
            window.setCentralWidget(view)
            #window.resize(*view.size())
            window.show()
            view.horizontalHeader().setResizeMode(QHeaderView.Interactive)
  
    def TemperaturePlot(self):
        self.mode = self.TemperaturePlotMode
        self.clearGraphs()
        data = self.data
        x_data = []
        for row in data:
            stamp = row[0].split('/') + row[1].split(':')
            stamp= map(int,stamp) + [0,0,0]
            x_data.append(time.mktime(stamp))
        N = len(data[0]) - 2
        for i in xrange(N):
            graph = self.addGraph()
            pen = QPen(QColor(*hex_to_rgb(ColorValues[i])))
            graph.setPen(pen)
            y_data = [float(row[i+2]) for row in data]
            graph.setData(x_data, y_data)
        self.xAxis.setTickLabelType(QCPAxis.ltDateTime)
        self.xAxis.setDateTimeFormat("hh:mm:ss")
        self.setInteraction(QCP.iSelectPlottables)
        #self.xAxis.setRange(x_data[0],x_data[-1])
        #self.yAxis.setRange(20,30)
        self.rescaleAxes()
        self.replot()