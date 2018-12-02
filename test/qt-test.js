var qt = require('qt');
var repl = require('repl');

app = new qt.QApplication;
timer = setInterval(app.processEvents, 0);

ui = new qt.QUiLoader();
win = ui.load(__dirname + '/test.ui');
win.show();
//r = qt.QMessageBox.warning('A','B',qt.STANDARDBUTTON.YES,qt.STANDARDBUTTON.NO,qt.STANDARDBUTTON.YES);
//console.log(r);

horizontalSlider = win.findChild('horizontalSlider');
horizontalSlider.off('valueChanged');
horizontalSlider.on('valueChanged',function(){console.log(this.value());})

repl.start('> ');
//clearInterval(timer);