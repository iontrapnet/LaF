#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys,os
from .Viewer import *
from PyQt4.QtGui import *
from PyQt4.QtCore import *

class FileView(QListView):
    def __init__(self, parent):
        QListView.__init__(self,parent)
        self.setContextMenuPolicy(Qt.DefaultContextMenu)
    
    def onShowAction(self):
        index = self.selectionModel().currentIndex()
        path = self.model().filePath(index)
        explorer = 'explorer.exe'
        command = explorer + ' /select,' + QDir.toNativeSeparators(path)
        QProcess.startDetached(command)
        
    def contextMenuEvent(self, event):
        pos = event.pos()
        index = self.indexAt(pos)
        path = self.model().filePath(index)
        info = QFileInfo(path)
        menu = QMenu()
        openAction = menu.addAction("Open By System")
        nameAction = menu.addAction("Copy File Name")
        pathAction = menu.addAction("Copy Full Path")
        showAction = QAction("Show In Explorer",menu,triggered=self.onShowAction)
        menu.addAction(showAction)
        quitAction = menu.addAction("Quit")
        action = menu.exec_(self.mapToGlobal(pos))
        if action == openAction:
            QDesktopServices.openUrl(QUrl.fromLocalFile(path))
        elif action == nameAction:
            clipboard = QApplication.clipboard()
            clipboard.setText(info.baseName())
        elif action == pathAction:
            clipboard = QApplication.clipboard()
            clipboard.setText(path)
        elif action == quitAction:
            qApp.quit()
        event.accept()
    
class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        self.resize(900, 600)
        self.setWindowTitle('Data Explorer')
        exit = QAction('Exit', self)
        exit.setShortcut('ESC')
        exit.setStatusTip('Exit') 
        self.connect(exit, SIGNAL('triggered()'), SLOT('close()'))
        embed = QAction('Embed', self)
        embed.setShortcut('F3')
        embed.setStatusTip('Embed')
        self.connect(embed, SIGNAL('triggered()'), self.embed_content)
        popup = QAction('Popup', self)
        popup.setShortcut('F4')
        popup.setStatusTip('Popup')
        self.connect(popup, SIGNAL('triggered()'), self.popup_content)

        self.statusBar()
        menubar = self.menuBar()
        file = menubar.addMenu('&File')
        file.addAction(exit)
        preview = menubar.addMenu('&Preview')
        preview.addAction(embed)
        preview.addAction(popup)

        self.mainWidget = QWidget(self)
        self.setCentralWidget(self.mainWidget)

        self.fileBrowserWidget = QWidget(self)
        self.browserSplitter = QSplitter()
        
        self.dirModel = QFileSystemModel()
        self.dirModel.setFilter(QDir.NoDotAndDotDot | QDir.AllDirs)
        
        self.dirView = QTreeView(parent=self);
        self.dirView.setModel(self.dirModel)
        self.dirView.setHeaderHidden(True)
        self.dirView.hideColumn(1)
        self.dirView.hideColumn(2)
        self.dirView.hideColumn(3)
        self.browserSplitter.addWidget(self.dirView)
        self.browserSplitter.setStretchFactor(0,0)
        
        self.dirSelection = self.dirView.selectionModel()
        self.dirSelection.selectionChanged.connect(self.select_dir)
        
        self.fileView = FileView(parent=self)
        self.fileView.doubleClicked.connect(self.dbclick_file)
        self.fileView.installEventFilter(self)
        self.browserSplitter.addWidget(self.fileView)
        self.browserSplitter.setStretchFactor(1,0)
        
        self.contentWindow = self
        self.contentWidget = QTableView(parent=self.contentWindow)
        self.browserSplitter.addWidget(self.contentWidget)
        self.browserSplitter.setStretchFactor(2,0)
        self.browserSplitter.setSizes([100,200,600])
                  
        hbox = QHBoxLayout()
        hbox.addWidget(self.browserSplitter)
        self.fileBrowserWidget.setLayout(hbox)

        vbox_main = QVBoxLayout(self.mainWidget)
        vbox_main.addWidget(self.fileBrowserWidget)       
        vbox_main.setContentsMargins(0,0,0,0)
        #self.setLayout(vbox_main)

    def set_path(self, path=""):
        self.dirModel.setRootPath('')
        self.dirView.setCurrentIndex(self.dirModel.index(path))
        
    def current_path(self):
        index = self.fileSelection.currentIndex()
        fileModel = self.fileView.model()
        path = fileModel.filePath(index)
        return path
        
    def embed_content(self):
        if self.contentWindow == self:
            return
        self.contentWindow.close()
        self.contentWindow = self
        self.contentWidget.setParent(self.contentWindow)
        self.browserSplitter.addWidget(self.contentWidget)
        self.browserSplitter.setStretchFactor(2,0)
        self.browserSplitter.setSizes([100,200,600])
           
    def popup_content(self):
        if self.contentWindow != self:
            return
        self.contentWindow = QMainWindow(parent=self)
        self.contentWindow.setWindowTitle(self.current_path())
        self.contentWindow.resize(600, 600)
        self.contentWidget.close()
        self.contentWidget.setParent(self.contentWindow)
        self.contentWidget.show()
        self.contentWindow.setCentralWidget(self.contentWidget)
        self.contentWindow.show()
        
    def replace_content(self, view):
        self.contentWidget.close()
        self.contentWidget.setParent(None)
        self.contentWidget.deleteLater()
        self.contentWidget = view
        self.browserSplitter.addWidget(view)
        self.browserSplitter.setStretchFactor(2,0)
        sizes = self.browserSplitter.sizes()
        sizes[2] = 2*(sizes[0] + sizes[1])
        self.browserSplitter.setSizes(sizes)
        
    def select_dir(self, current):
        index = self.dirSelection.currentIndex()
        if self.dirModel.fileInfo(index).isDir():
            path = self.dirModel.filePath(index)
            model = QFileSystemModel()
            model.setFilter(QDir.AllEntries)
            model.setRootPath(path)
            view = self.fileView
            view.setModel(model)
            view.setRootIndex(model.index(path))
            self.fileSelection = view.selectionModel()
            self.fileSelection.selectionChanged.connect(self.select_file)
    
    def content_view(self, clazz):
        if self.contentWindow != self:
            self.contentWindow.setWindowTitle(self.current_path())
        if not isinstance(self.contentWidget,clazz):
            view = clazz(parent=self.contentWindow)
            self.replace_content(view)
        return self.contentWidget
    
    def separate_view(self, clazz):
        window = QMainWindow(parent=self)
        #window = QMdiSubWindow(parent=self)
        window.setWindowTitle(self.current_path())
        window.resize(600,600)
        view = clazz(parent=window)
        window.setCentralWidget(view)
        #window.setWidget(view)
        #window.setOption(QMdiSubWindow.RubberBandResize)
        #window.setOption(QMdiSubWindow.RubberBandMove)
        window.show()
        return view
    
    def select_file(self):
        viewer = Viewer(self.current_path(), self.content_view)
        viewer.handle_file()
        
    def dbclick_file(self, index):
        fileModel = self.fileView.model()
        path = fileModel.filePath(index)
        if fileModel.fileInfo(index).isDir():
            fileModel.setRootPath(path)
            index = fileModel.index(path)
            self.fileView.setRootIndex(index)
            self.fileView.setCurrentIndex(index)
            index = self.dirModel.index(path)
            self.dirView.setCurrentIndex(index)
        else:
            viewer = Viewer(self.current_path(), self.separate_view)
            viewer.handle_file()
    
    def eventFilter(self, object, event):
        if isinstance(object,QAbstractItemView):
            if event.type() == QEvent.KeyPress and \
            event.matches(QKeySequence.InsertParagraphSeparator):
                index = object.currentIndex()
                if object == self.fileView:
                    object.doubleClicked.emit(index)
                    #self.dbclick_file(index)
                else:  
                    object.clicked.emit(index)
                return True
        return False

app = QApplication(sys.argv)
main = MainWindow()
main.show()
main.set_path('G:/Data')
#main.set_path('E:/Data/Temperature')

sys.exit(app.exec_())