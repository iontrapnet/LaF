var zerorpc = require("zerorpc");

var server = new zerorpc.Server({
    echo: function(args, reply) {
        console.log(args);
        reply(null, args);
        /*require('http').get('http://www.baidu.com',function(res){
            html = '';
            res.on('data',function(data){
                html += data;
            }).on('end',function(){
                reply(null, html);
                // chcp 65001
            });
        });*/
    }
});

server.bind("tcp://0.0.0.0:4242");