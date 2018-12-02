var repl = require('nesh');
mqtt = require('mqtt');
zmq = require('zeromq');
LV = require('../LabVIEW');

setInterval(function(){
LV.Queue('Test', null, function (err, res) {
    console.log(res);
})
},1000);

mqttc = mqtt.connect({host:'127.0.0.1',port:1883});
mqttc.on('connect',function() {
    mqttc.subscribe('Test',{qos:1});
});
mqttc.on('message',function(topic,data){
    console.log(topic,data.toString());
});

zmqc = new zmq.Socket('sub');
zmqc.connect('tcp://127.0.0.1:5000');
zmqc.subscribe('Test');
zmqc.on('message',function(msg){
    console.log(msg.toString());
});

repl.start('> ');