var repl = require('nesh');
LV = require('../LabVIEW');
vi = __dirname + '\\icsTest.vi';
ctrls = {};
names = {};
configs = [];
steps = [];
voltages = [];

setInterval(function() {
    LV.Get(vi,'ExecState',function(err,res) {
        if (res == 'eIdle') process.exit(0);
    });
},1000);

LVSock = require('dgram').createSocket('udp4',{reuseAddr:true});
LVSock.bind(10000);
LVSock.setBroadcast(true);
LVSock.setMulticastTTL(128);
LVSock.addMembership('224.0.0.0');
LVSock.on('message', function(msg,err) {
    console.log(msg.toString());
    var ctrl = msg.toString().trim().split(' ')[1];
    LV.CtrlGetValue(ctrl,function(err,res) {
        var name = names[ctrl];
        //console.log(name,' ',res);
        Test(ctrl,name,res);
    });
});

LV.GetAllCtrls(vi, function (err, res) {
    ctrls = res;
    for (var k in ctrls) {
        names[ctrls[k]] = k;
        var name = k.split(' ');
        switch (name[0]) {
        case 'Vset':
            getVoltage(name[1], name[2]);
            break;
        case 'Config':
            LV.CtrlGetValue(ctrls[k],function(err,res) {
                configs[name[1]] = eval('(' + res + ')');
            });
            break;
        case 'Steps':
            LV.CtrlGetValue(ctrls[k],function(err,res) {
                steps[name[1]] = res;
            });
            break;
        }
    }
});

function Test(ctrl,name,val) {
    if (name == 'OK' || name == 'Cancel' || name == 'Stop') {
        process.exit(0);
    }
    name = name.split(' ');
    switch (name[0]) {
    case 'Vset':
        setVoltage(name[1],val);
        break;
    case 'Config':
        configs[name[1]] = eval('(' + val + ')');
        break;
    case 'Steps':
        var config = configs[name[1]];
        var delta = val - steps[name[1]];
        for (var ch in config) {
            var v = voltages[ch]+config[ch]*delta;
            setVoltage(ch,v);
            LV.CtrlSetValue(ctrls['Vset '+ch],v,null);
        }
        steps[name[1]] = val;
        break;
    }
}

var request = require('request');
icsConnect = require('./icsConnect');
icsConnect.host = '192.168.1.6';
icsConnect.wsUri = 'ws://' + icsConnect.host + ':8080';
icsConnect.xhrUri = 'http://' + icsConnect.host + '/api';
icsConnect.xhrAutoFallback = false;
icsConnect.debug = false;
request.get(icsConnect.xhrUri + '/login/admin/password', function(err,res,body) {
    if (err) {
        
    }
    var json = JSON.parse(body);
    icsConnect.iCSsessionId = json.i;
    icsConnect.connect();
});

function setVoltage(ch,v) {
    ch = ch.split('-');
    if (ch.length == 1) ch.unshift('0');
    icsConnect.appendPacket('setItem', '0', ch[0], ch[1], 'Control.voltageSet',''+v.toFixed(1),'V');
    icsConnect.transmit();
    voltages[ch] = v;
}

function getVoltage(ch) {
    ch = ch.split('-');
    if (ch.length == 1) ch.unshift('0');
    setInterval(function () {
        icsConnect.appendPacket('getItem', '0', ch[0], ch[1], 'Status.voltageMeasure','','');
        icsConnect.transmit();
    },500);
}

icsConnect.onRefresh = function (data,id) {
    switch(data.i) {
    case 'Status.voltageMeasure':
        var ch = data.p.a + '-' + data.p.c;
        var v = parseFloat(data.v);
        LV.CtrlSetValue(ctrls['Vact '+ch],v,null);
        if (!voltages[ch]) {
            voltages[ch] = v;
            LV.CtrlSetValue(ctrls['Vset '+ch],v,null);
        }
        break;
    }
};

repl.start('> ');