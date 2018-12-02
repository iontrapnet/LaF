// scriptfile encoding: utf8

var ffi = require("ffi");
//var SHcore = new ffi.Library("SHcore", { "SetProcessDpiAwareness" : [ "int", [ "int" ] ] });
//console.log(SHcore.SetProcessDpiAwareness(1));
var User32 = new ffi.Library("User32", { "SetProcessDPIAware" : [ "int8", [ ] ] });
console.log(User32.SetProcessDPIAware());

var qt  = require('qt');
var fs  = require('fs');

global.editor = {
	app_name:"SimpleEdit",
	app_version:"0.3",
	
	mainWindow:null,
	menuBar:null,
	toolBar:null,
	statusBar:null,
	plainTextEdit:null,
	printer:null,
	dialog:null,
	
	curFile:"",
	isModified:false,
	searchStr:"",
		
	menus:{},
	actions:{},
	
	settings: {//defaults
	  "state": 0,
	  "toolbar": true,
	  "statusbar": true,
	  "openlast": true,
	  "encoding": "binary",
	  "font": "Courier New",
	  "fontsize": 10,
	  "tabstopwidth": 30
	}
};


// METHODS ///////////////////////////////////////

/**
 * editor.readSettings
 * Reads settings from registry, returns object (or false).
 */
editor.readSettings = function(){
	var str = false;
	try{
		str = fs.readFileSync(__dirname + "/config.txt", "utf8");
	}catch(e){};
	return str ? JSON.parse(str) : false;
}

/**
 * editor.writeSettings
 * Writes settings to registry (as JSON string).
 */
editor.writeSettings = function(){
	//var settings = {};
	editor.settings.state = editor.mainWindow.windowState();
	editor.settings.toolbar = editor.toolBar.isVisible();
	editor.settings.statusbar = editor.statusBar.isVisible();
	editor.settings.lastfile = editor.curFile;
	var jsonString = JSON.stringify(editor.settings, null, "  ");
	fs.writeFileSync(__dirname + "/config.txt", jsonString, "utf8");
}

/**
 * editor.newFile
 * Handles New action.
 */
editor.newFile = function(){
  if (editor.maybeSave()) {
		editor.plainTextEdit.clear();
   	editor.setCurrentFile("");
  }
}

/**
 * editor.open
 * Handles Open action.
 */
editor.open = function(){
  if (editor.maybeSave()){
		var fn = qt.QFileDialog.getOpenFileName();
		if (fn) editor.loadFile(fn);
	}
}

/**
 * editor.save
 * Handles Save action.
 */
editor.save = function(){
  if (!editor.curFile){
		return editor.saveAs();
  } else {
		return editor.saveFile(editor.curFile);
  }
}

/**
 * editor.saveAs
 * Handles SaveAs action.
 */
editor.saveAs = function(){
	var fn = qt.QFileDialog.getSaveFileName(0,editor.curFile);
	if (!fn) return false;
	return editor.saveFile(fn);
}

/**
 * editor.maybeSave
 */
editor.maybeSave = function(){
  if (editor.isModified){
  	var ret = qt.QMessageBox.warning(
			"Editor",
			"The document has been modified.\nDo you want to save your changes?",
			qt.STANDARDBUTTON.SAVE,qt.STANDARDBUTTON.DISCARD,qt.STANDARDBUTTON.CANCEL
		);
    if (ret == qt.STANDARDBUTTON.SAVE)
			return editor.save();
    else if (ret == qt.STANDARDBUTTON.CANCEL)
			return false;
			
  }
  return true;
}

/**
 * editor.loadFile
 * Loads given file.
 *
 * @param fn {String}
 */
editor.loadFile = function(fn){
	var enc = editor.settings.encoding;
	// check BOM
	var buf = new Buffer(3);
	var fd = fs.openSync(fn, "r");
	fs.readSync(fd, buf, 0, 3, 0);
	fs.closeSync(fd);
	
	if (buf[0]==0xef&&buf[1]==0xbb&buf[2]==0xbf) enc = "utf8";
	else if (buf[0]==0xff&&buf[1]==0xfe) enc = "utf16le";
	else if (buf[0]==0xfe&&buf[1]==0xff) enc = null; // utf16be: read as raw buffer
		
	fs.readFile(fn, enc, function (err, data) {
	  if (err) {
	    //throw err;
	    qt.QMessageBox.warning("Editor", "Cannot read file "+fn);
	    return;
	  }
	  
		if (buf[0]==0xfe&&buf[1]==0xff){// utf16be
			// swap bytes
			var a, len = data.length;
			for (var i = 0; i < len; i += 2) {
			  a = data[i];
			  data[i] = data[i+1];
			  data[i+1] = a;
			}
			data = data.toString("utf16le");
		}
		
		editor.plainTextEdit.setPlainText(data);
		
		// convert MARKDOWN to HTML
//		Converter = require("./Markdown.Converter").Converter;
//		var conv = new Converter();
//		var html = conv.makeHtml(data);
//		editor.plainTextEdit.setPlainText("");
//		editor.plainTextEdit.appendHtml(html);
		
		editor.setCurrentFile(fn);
		editor.statusBar.showMessage("File loaded.", 2000);
	});
}

/**
 * editor.saveFile
 * Saves given file.
 *
 * @param fn {String}
 */
editor.saveFile = function(fn){
  var str = editor.plainTextEdit.toPlainText();
	fs.writeFile(fn, str, editor.encoding, function (err) {
	  if (err) {
	    //throw err;
			qt.QMessageBox.warning("Editor", "Cannot write file "+fn);
			return false;
	  }
	  editor.setCurrentFile(fn);
	  editor.statusBar.showMessage("File saved.", 2000);
	});  
}

/**
 * editor.setCurrentFile
 * Sets current file to the given file.
 *
 * @param fn {String}
 */
editor.setCurrentFile = function(fn){
  editor.curFile = fn;
	editor.isModified = false;
  var shownName = editor.curFile?editor.curFile:"untitled.txt";
  editor.mainWindow.setWindowTitle(editor.app_name + " - " + shownName);
}

/**
 * editor.exit
 * Handles Exit action.
 */
editor.exit = function(){
  if (editor.maybeSave()) {
		editor.writeSettings(); //-> saves wrong settings! isVisible always false
		process.exit();
  }
}


// LOAD/CREATE SETTINGS //////////////////////////

var settings = editor.readSettings();
if (settings) editor.settings = settings


// START /////////////////////////////////////////

editor.app = new qt.QApplication;
editor.mainWindow = new qt.QMainWindow;


// CREATE MENUBAR ////////////////////////////////

editor.menuBar = new qt.QMenuBar;


// CREATE TOOLBAR ////////////////////////////////

editor.toolBar = new qt.QToolBar;
editor.toolBar.setWindowTitle("ToolBar");
editor.toolBar.setVisible(editor.settings.toolbar);


// CREATE STATUSBAR //////////////////////////////

editor.statusBar = new qt.QStatusBar;
editor.statusBar.setVisible(editor.settings.statusbar);


// CREATE PLAINTEXTEDIT //////////////////////////

editor.plainTextEdit = new qt.QPlainTextEdit;
//if (editor.settings.font && editor.settings.fontsize) 
	editor.plainTextEdit.setFont(editor.settings.font, editor.settings.fontsize);
//if (editor.settings.tabstopwidth) 
	editor.plainTextEdit.setTabStopWidth(editor.settings.tabstopwidth);
editor.plainTextEdit.on("textChanged", function(){
	editor.isModified = true;
	var shownName = editor.curFile;
	if (!editor.curFile) shownName = "untitled.txt";
	editor.mainWindow.setWindowTitle(editor.app_name + " - " + shownName + "*");
});


// CREATE DIALOG OBJECT //////////////////////////

editor.dialog = new qt.QDialog;


// CONFIGURE MENUBAR /////////////////////////////

// MENU FILE
editor.menus.menu_file = editor.menuBar.addMenu('&File');

// FILE:NEW
editor.actions.action_new = editor.menus.menu_file.addAction('&New');
editor.actions.action_new.on('triggered', editor.newFile);
editor.actions.action_new.setIcon(__dirname + "/assets/images/new.png");
editor.actions.action_new.setShortcut("Ctrl+N");
editor.actions.action_new.setStatusTip("Create a new file");

// FILE:OPEN
editor.actions.action_open = editor.menus.menu_file.addAction('&Open');
editor.actions.action_open.on('triggered', editor.open);
editor.actions.action_open.setIcon(__dirname + "/assets/images/open.png");
editor.actions.action_open.setShortcut("Ctrl+O");
editor.actions.action_open.setStatusTip("Open an existing file");

// FILE:SAVE
editor.actions.action_save = editor.menus.menu_file.addAction('&Save');
editor.actions.action_save.on('triggered', editor.save);
editor.actions.action_save.setIcon(__dirname + "/assets/images/save.png");
editor.actions.action_save.setShortcut("Ctrl+S");
editor.actions.action_save.setStatusTip("Save the document to disk");

// FILE:SAVEAS
editor.actions.action_saveas = editor.menus.menu_file.addAction('Save &As...');
editor.actions.action_saveas.on('triggered', editor.saveAs);
editor.actions.action_saveas.setStatusTip("Save the document under a new name");

editor.menus.menu_file.addSeparator();

// FILE:PAGESETUP
editor.actions.action_pagesetup = editor.menus.menu_file.addAction('Page Setup');
editor.actions.action_pagesetup.on('triggered', function(){
	if (!editor.printer) editor.printer = new qt.QPrinter;
	if (editor.printer){
    editor.dialog.pageSetupDialog(editor.printer, 0);
	}
});

// FILE:PRINTPREVIEW
editor.actions.action_printpreview = editor.menus.menu_file.addAction('&Print Preview');
editor.actions.action_printpreview.on('triggered', function(){
	if (!editor.printer) editor.printer = new qt.QPrinter;
	if (editor.printer){
		// parent=0, printer=0, widget=0, flags
		editor.dialog.printPreviewDialog(0, editor.printer, editor.plainTextEdit);
  }
});


// FILE:PRINT
editor.actions.action_print = editor.menus.menu_file.addAction('&Print');
editor.actions.action_print.on('triggered', function(){
	if (!editor.printer) editor.printer = new qt.QPrinter;
	if (editor.printer){
		var ok = editor.dialog.printDialog(editor.printer, 0);
  	if (ok) editor.plainTextEdit.print(editor.printer);
  }
});
editor.actions.action_print.setShortcut("Ctrl+P");
editor.actions.action_print.setStatusTip("Print current text");
  
editor.menus.menu_file.addSeparator();

// FILE:EXIT
editor.actions.action_exit = editor.menus.menu_file.addAction('E&xit');
editor.actions.action_exit.on('triggered', editor.exit);
editor.actions.action_exit.setShortcut("Ctrl+Q");
editor.actions.action_exit.setStatusTip("Exit the application");


// MENU EDIT
editor.menus.menu_edit = editor.menuBar.addMenu('&Edit');


// EDIT:UNDO
editor.actions.action_undo = editor.menus.menu_edit .addAction('&Undo');
editor.actions.action_undo.on('triggered', function(){
	editor.plainTextEdit.undo();
});
editor.actions.action_undo.setShortcut("Ctrl+Z");

editor.menus.menu_edit.addSeparator();

// EDIT:CUT
editor.actions.action_cut = editor.menus.menu_edit.addAction('Cu&t');
editor.actions.action_cut.on('triggered', function(){
	editor.plainTextEdit.cut();
});
editor.actions.action_cut.setIcon(__dirname + "/assets/images/cut.png");
editor.actions.action_cut.setShortcut("Ctrl+X");
editor.actions.action_cut.setStatusTip("Cut the current selection's contents to the clipboard");

editor.actions.action_copy = editor.menus.menu_edit.addAction('&Copy');
editor.actions.action_copy.on('triggered', function(){
	editor.plainTextEdit.copy();
});
editor.actions.action_copy.setIcon(__dirname + "/assets/images/copy.png");
editor.actions.action_copy.setShortcut("Ctrl+C");
editor.actions.action_copy.setStatusTip("Copy the current selection's contents to the clipboard");

editor.actions.action_paste = editor.menus.menu_edit.addAction('&Paste');
editor.actions.action_paste.on('triggered', function(){
	editor.plainTextEdit.paste();
});
editor.actions.action_paste.setIcon(__dirname + "/assets/images/paste.png");
editor.actions.action_paste.setShortcut("Ctrl+V");
editor.actions.action_paste.setStatusTip("Paste the clipboard's contents into the current selection");

editor.menus.menu_edit.addSeparator();

// EDIT:CLEAR
editor.actions.action_clear = editor.menus.menu_edit.addAction('Clear');
editor.actions.action_clear.on('triggered', function(){
	editor.plainTextEdit.clear();
});

// EDIT:FIND
editor.actions.action_find = editor.menus.menu_edit.addAction('&Find');
editor.actions.action_find.on('triggered', function(){
	editor.searchStr = editor.dialog.inputDialog(0, "Find String", "Search for:", "");
	if (editor.searchStr) editor.plainTextEdit.find(editor.searchStr);
});
editor.actions.action_find.setShortcut("Ctrl+F");

// EDIT:FINDAGAIN
editor.actions.action_findagain = editor.menus.menu_edit.addAction('Find again');
editor.actions.action_findagain.on('triggered', function(){
	if (editor.searchStr) editor.plainTextEdit.find(editor.searchStr);
});

editor.menus.menu_edit.addSeparator();

// EDIT:SELECT ALL
editor.actions.action_selectall = editor.menus.menu_edit.addAction('&Select All');
editor.actions.action_selectall.on('triggered', function(){
	editor.plainTextEdit.selectAll();
});
editor.actions.action_selectall.setShortcut("Ctrl+A");


// MENU VIEW

editor.menus.menu_view = editor.menuBar.addMenu('&View');

// VIEW:SHOWTOOLBAR
editor.actions.action_showtoolbar = editor.menus.menu_view.addAction('Show ToolBar');
editor.actions.action_showtoolbar.setCheckable(true);
editor.actions.action_showtoolbar.setChecked(editor.settings.toolbar);
editor.actions.action_showtoolbar.on('toggled', function(checked){
	editor.toolBar.setVisible(checked);
});

// VIEW:SHOWSTATUSBAR
editor.actions.action_showstatusbar = editor.menus.menu_view.addAction('Show StatusBar');
editor.actions.action_showstatusbar.setCheckable(true);
editor.actions.action_showstatusbar.setChecked(editor.settings.statusbar);
editor.actions.action_showstatusbar.on('toggled', function(checked){
	editor.statusBar.setVisible(checked);
});

// MENU HELP
editor.menus.menu_help = editor.menuBar.addMenu('&Help');
editor.actions.action_about = editor.menus.menu_help.addAction('About');
editor.actions.action_about.on('triggered', function(){
	qt.QMessageBox.about("About", "<b>"+editor.app_name+" v"+editor.app_version+"</b><br><br>This application demonstrates how to write modern GUI applications with a menu bar, toolbars, and a status bar in pure <a href='https://en.wikipedia.org/wiki/JavaScript'>JavaScript</a>, using <a href='http://nodejs.org/'>Node.js</a> and the <a href='https://github.com/arturadib/node-qt#readme'>Node-Qt Addon</a>.");
});
editor.actions.action_aboutqt = editor.menus.menu_help.addAction('About Qt');
editor.actions.action_aboutqt.on('triggered', function(){
	qt.QMessageBox.aboutQt('About Qt');
});

editor.menus.menu_help.addSeparator();

editor.actions.action_appscript = editor.menus.menu_help.addAction("Show app script");
editor.actions.action_appscript.on('triggered', function(){
	if (editor.maybeSave()){
		editor.loadFile(__filename);
	}
});

// CONFIGURE TOOLBAR /////////////////////////////

editor.toolBar.on("visibilityChanged", function(vis){
	// update menu
	editor.actions.action_showtoolbar.setChecked(vis);
});
editor.toolBar.addAction(editor.actions.action_new);
editor.toolBar.addAction(editor.actions.action_open);
editor.toolBar.addAction(editor.actions.action_save);
editor.toolBar.addSeparator();
editor.toolBar.addAction(editor.actions.action_cut);
editor.toolBar.addAction(editor.actions.action_copy);
editor.toolBar.addAction(editor.actions.action_paste);


// CONFIGURE MAINWINDOW //////////////////////////

editor.mainWindow.setAcceptDrops(function(data){
	if (data['application/x-qt-windows-mime;value="FileName"']){//WIN
		if (editor.maybeSave()){
			var fn = data['application/x-qt-windows-mime;value="FileName"'].toString();
			editor.loadFile(fn);
		}
	}else if (data['text/uri-list']){//MAC
		if (editor.maybeSave()){
			var flist = data['text/uri-list'].toString().split("\r\n");
			if (flist.length){
      	var fn = flist[0];
				if (fn.substr(0,7)=='file://') fn = fn.substr(7);
				editor.loadFile(fn);
			}
		}
	}
});
editor.mainWindow.setWindowIcon(__dirname + "/assets/images/icon.png");
editor.mainWindow.setMenuBar(editor.menuBar);
editor.mainWindow.addToolBar(editor.toolBar);
editor.mainWindow.setCentralWidget(editor.plainTextEdit);
editor.mainWindow.setStatusBar(editor.statusBar);
editor.mainWindow.on('closeEvent', editor.exit);
editor.mainWindow.resize(640, 480);
editor.mainWindow.setWindowState(editor.settings.state);
if (process.argv.length>2){
	try{
		editor.loadFile(process.argv[2]);
	}catch(e){
		editor.mainWindow.setWindowTitle(editor.app_name + " - untitled.txt");
	};
}else if (editor.settings.openlast && editor.settings.lastfile){
	try{
		editor.loadFile(editor.settings.lastfile);
	}catch(e){
		editor.mainWindow.setWindowTitle(editor.app_name + " - untitled.txt");
	};
}else
	editor.mainWindow.setWindowTitle(editor.app_name + " - untitled.txt");
editor.mainWindow.show();


// JOIN NODE'S EVENT LOOP ////////////////////////

setInterval(editor.app.processEvents, 0);
