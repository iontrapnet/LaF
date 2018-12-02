#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys,os,math,random,csv
from .Plotter import *
from .DataParser import *
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from PyQt4.Qsci import QsciScintilla

#from Test import *

class Viewer:
    def __init__(self, path, method):
        self.path = path
        self.method = method
    
    def handle_data(self, data):
        model = QStandardItemModel()
        for row in data:
            items = [
                QStandardItem(field)
                for field in row
            ]
            model.appendRow(items)
        self.handle_table(model)
        
    def handle_table(self, model):
        view = self.method(QTableView)
        view.setModel(model)
        view.setAlternatingRowColors(True)
        view.horizontalHeader().setResizeMode(QHeaderView.Stretch)
        view.resizeRowsToContents()
        #view.setEditTriggers(QAbstractItemView.NoEditTriggers)
        try: view.clicked.disconnect()
        except TypeError: pass
        view.close()
        view.show()
        view.horizontalHeader().setResizeMode(QHeaderView.Interactive)
        
    def handle_file(self):
        path = self.path
        info = QFileInfo(path)
        if info.isDir():
            model = QFileSystemModel()
            model.setFilter(QDir.NoDotAndDotDot | QDir.AllEntries)
            model.setRootPath(path)
            view = self.method(QTableView)
            view.setModel(model)
            view.setRootIndex(model.index(path))
            try: view.clicked.disconnect()
            except TypeError: pass
            view.clicked.connect(self.click_dircontent(model))
            view.setSortingEnabled(True)
            view.setSelectionBehavior(QTableView.SelectRows)
            view.horizontalHeader().setResizeMode(QHeaderView.Stretch)
            view.resizeRowsToContents()
            view.close()
            view.show()
            view.horizontalHeader().setResizeMode(QHeaderView.Interactive)
            return
        ext = info.suffix()
        if ext == 'txt' or ext == 'log':
            if ext == 'txt' and info.dir().dirName() == 'Wave':
                with open(path, "rb") as input:
                    data = [row for row in csv.reader(input)]
                self.handle_data(data)
                return
            parser = DataParser()
            text = open(path, "r").read()
            data = parser.parseText(text)
            model = QStandardItemModel()
            for key in data:
                items = [
                    QStandardItem(key),
                    QStandardItem(data[key])
                ]
                model.appendRow(items)
            if 'Counts' in data:
                view = self.method(Plotter)
                view.path = path
                view.data = data
                view.CountsPlot()
                #view = self.method(Test)
            else:
                self.handle_table(model)  
        elif ext == 'csv':
            with open(path, "rb") as input:
                data = [row for row in csv.reader(input)]
            if info.dir().dirName() == 'Temperature':
                view = self.method(Plotter)
                view.path = path
                view.data = data
                view.TemperaturePlot()
            else:
                self.handle_data(data)
        elif ext == 'htm' or ext == 'html':
            view = self.method(QTextBrowser)
            view.setSource(QUrl.fromLocalFile(path))
        else:
            view = self.method(QsciScintilla)
            view.setText(open(path).read())
       
    def click_dircontent(self, model):
        def func(index):
            path = model.filePath(index)
            QDesktopServices.openUrl(QUrl.fromLocalFile(path))
        return func