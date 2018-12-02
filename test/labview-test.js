var repl = require('nesh');
LV = require('../LabVIEW');
ctrls = {};
names = {};
data = [];

LVSock = require('dgram').createSocket('udp4');
LVSock.bind(10000);
LVSock.on('message', function(msg,err) {
    var ctrl = msg.toString().trim().split(' ')[1];
    LV.CtrlGetValue(ctrl,function(err,res) {
        console.log(names[ctrl],' ',res);
    });
});

LV.GetAllCtrls(__dirname + '\\Test.UDP.vi', function (err, res) {
    ctrls = res;
    for (var k in ctrls) names[ctrls[k]] = k;
});

repl.start('> ');