var repl = require('repl');
var request = require('request');
var qt = require('qt');

icsConnect = require('./icsConnect');
icsConnect.host = '192.168.32.3';
icsConnect.wsUri = 'ws://' + icsConnect.host + ':8080';
icsConnect.xhrUri = 'http://' + icsConnect.host + '/api';
icsConnect.xhrAutoFallback = false;
icsConnect.debug = false;
//icsConnect.onMessage = console.log;
icsConnect.onRefresh = console.log;
request.get(icsConnect.xhrUri + '/login/admin/password', function(err,res,body) {
    var json = JSON.parse(body);
    icsConnect.iCSsessionId = json.i;
    icsConnect.connect();
});

//icsConnect.getAllItems();

//icsConnect.appendPacket('getItem', "0", "0", "0", "Status.voltageMeasure","","");
//icsConnect.transmit();

//icsConnect.appendPacket('setItem', "0", "0", "0", "Control.voltageSet","5","V");
//icsConnect.transmit();

app = new qt.QApplication;
timer = setInterval(app.processEvents, 0);
ui = new qt.QUiLoader();
win = ui.load('ics.ui');
win.show();

function getVoltage(a,b) {
    icsConnect.appendPacket('getItem', "0", "2", "1", "Status.voltageMeasure","","");
    icsConnect.appendPacket('getItem', "0", "2", "3", "Status.voltageMeasure","","");
    icsConnect.transmit();
}

function setVoltage(a,b) {
    icsConnect.appendPacket('setItem', "0", "2", "1", "Control.voltageSet",""+ratio*a,"V");
    icsConnect.appendPacket('setItem', "0", "2", "3", "Control.voltageSet",""+ratio*b,"V");
    icsConnect.transmit();
}

var Va = 20;
var Vb = 20;
var ratio = 10;

plusplusBtn = win.findChild('plusplusBtn');
plusplusBtn.on('clicked',function(evt) {   
    setVoltage(++Va,++Vb);
});
minusminusBtn = win.findChild('minusminusBtn');
minusminusBtn.on('clicked',function(evt) {
    setVoltage(--Va,--Vb);
});
plusminusBtn = win.findChild('plusminusBtn');
plusminusBtn.on('clicked',function(evt) {   
    setVoltage(++Va,--Vb);
});
minusplusBtn = win.findChild('minusplusBtn');
minusplusBtn.on('clicked',function(evt) {
    setVoltage(--Va,++Vb);
});

repl.start('> ');
//clearInterval(timer);