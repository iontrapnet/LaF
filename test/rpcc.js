var zerorpc = require("zerorpc");

var client = new zerorpc.Client();
client.connect("tcp://127.0.0.1:4242");

client.invoke("echo", [1,2,3], function(error, res, more) {
    console.log(res);
});
client.invoke("echo", {1:1,2:2,a:3}, function(error, res, more) {
    console.log(res);
});