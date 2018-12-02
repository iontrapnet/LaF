/* ****************************************************
 * iseg Communication Server 2 - iCS2
 * iCSservice connection functions
 *
 * Copyright (c) 2012-2016 iseg Spezialelektronik GmbH
 * Internet: http://www.iseg-hv.com
 * E-Mail: info@iseg-hv.de
 **************************************************** */

var WebSocket = require('ws');
var _ = require('underscore');

var icsConnect = {
    xhrUpdateCycle: 0,
    wsUri: '"ws://127.0.0.1:8080"', // URI for websocket access (server where iCS service is running)
    xhrUri: "http://localhost/api", // URI for websocket gateway (mostly iCS Webserver)
    wsConnectionTimeout: 2000, // timeout for opening websocket connection in ms
    xhrTimeout: 60000, // XHR Timeout
    connectionMode: 'websocket', // Standard Connection Mode (websocket | HTTP), if websocket not availlable or timeout, HTTP (XHR) is used as fallback
    connected: false,
    xhrAutoFallback: true, // auto try XHR after Websockets failed
    debug: false, // log debug messages in console
    updateThreadTimeout: 1, // 1: fast updates, freezes probably on slow browsers/systems, 2 default, 3 or more for slow systems
    waitListSize: 0, //items to be operated
    user: null, // user currently logged in in web
    iCSsessionId: null, // sessionId which iCSservice delivers after login
    compression: false,
    nextMessageIsResponse: false,

    // Inits
    connectionError: false,
    packets: [],
    xhrUpdateInterval: -1,
    websocketIsGood: false,
    xhrIsGood: false,

    // Methods
    onMessage: function (message) {
//        console.log('please implement function onMessage(message)');
    },
    onResponse: function (message) {
//        console.log('please implement function onResponse(message)');
    },
    onSend: function (message) {
//        console.log('please implement function onSend(message)');
    },
    onRefresh: function (data, id) {
//        console.log('please implement function onRefresh(data,id)');
    },
    onInitConnection: function () {
//        console.log('please implement function onInitConnection()');
    },
    onConnect: function () {
//        console.log('please implement function onConnect()');
    },
    onDisconnect: function () {
//        console.log('please implement function onDisconnect()');
    },
    onError: function (message) {
//        console.log('please implement function onError(message)');
    },
    onInfo: function (message) {
//        console.log('please implement function onInfo(message)');
    },
    onMessageComplete: function (message) {
//        console.log('please implement function onMessageComplete()');
    },
    init: function () {
        //Deferred.define();
    },
    isConnected: function () {
        if (this.connected) {
            return true;
        } else {
            return false;
        }
    },

    // enables cyclic XHR polling
    enableXhrUpdates: function (cycle) {
        if (typeof cycle == 'undefined') {
            var cycle = this.xhrUpdateCycle;
        }
        if (icsConnect.xhrUpdateInterval == -1) {
            icsConnect.xhrUpdateInterval = setInterval(this.getUpdate, cycle);
        }
    },
    // disables cyclic XHR polling
    disableXhrUpdates: function () {
        if (icsConnect.xhrUpdateInterval != 'undefined') {
            clearInterval(icsConnect.xhrUpdateInterval);
            icsConnect.xhrUpdateInterval = -1;
        }
    },
    // initializes a first XHR connection, iCS service prepares polling queue
    openXhr: function () {
        if (this.webSocket != 'undefined') {
            delete this.webSocket;
        }
        this.connectionMode = 'HTTP';

        if (icsConnect.iCSsessionId == null) {
            icsConnect.login();
        }

        if (this.xhrUpdateCycle > 0) {
            this.enableXhrUpdates(icsConnect.xhrUpdateCycle);
        }
    },
    // sends an envelope via the XHR gateway to the iCS
    sendXhr: function (sendString) {
        var response = '';
        $.ajaxq('connect-http', {
            url: this.xhrUri + '/wsforward',
            type: "post",
            cache: false,
            timeout: this.xhrTimeout,
//            dataType: "text",
            data: {data: sendString}
        }).done(function (data) {
            if (this.debug) {
                console.time('processDataXhr');
            }
            icsConnect.processReceivedData(data);
            icsConnect.onResponse('');
            if (this.debug) {
                console.timeEnd('processDataXhr');
            }
        }).fail(function () {
//                icsConnect.disableXhrUpdates();
            icsConnect.onError('Connection Error C20', 'no connection');
            response = false;
        });
        return response;
    },

    // opens websocket connection
    openWebsocket: function () {
        this.disableXhrUpdates();
        // if user is running mozilla then use it's built-in WebSocket
        //window.WebSocket = window.WebSocket || window.MozWebSocket;
        try {
            this.webSocket = new WebSocket(this.wsUri);
        } catch (e) {
            if (this.xhrAutoFallback) {
                icsConnect.onError('Connection Error C22', 'iCSservice not available (Websocket connect failed), trying HTTP');
                icsConnect.websocketIsGood = false;
                setTimeout(function () {
                    icsConnect.connectXhr();
                }, 2000);
            }
            return false;
        }
        if (this.debug === true) {
            console.log("openWebsocket() webSocket:");
            console.log(this.webSocket);
        }
        this.webSocket.onopen = function (evt) {
            // reset timer, if websocket open event occured
            clearInterval(this.connectionTimer);
            this.websocketIsGood = true;
            if (icsConnect.iCSsessionId == null) {
                icsConnect.login();
            }
            this.onConnect(this.connectionMode);
            if (this.debug === true) {
                console.log("openWebsocket(): CONNECTED");
            }
            this.websocketSuccess = true;
        }.bind(this);
        this.webSocket.onclose = function (evt) {
            if (icsConnect.websocketIsGood) {
                this.onDisconnect('websocket');
            }
            if (this.xhrAutoFallback) {
                icsConnect.websocketIsGood = false;
                setTimeout(function () {
                    icsConnect.connectXhr();
                }, 2000);
            }
            if (this.debug === true) {
                console.log("openWebsocket(): DISCONNECTED");
            }
        }.bind(this);
        this.webSocket.onmessage = function (evt) {
            //this.processReceivedData('[' + evt.data + ']');
            if (this.debug) {
                console.time('processDataWebsocket');
            }
            this.processReceivedData(evt.data);
            if (this.debug) {
                console.timeEnd('processDataWebsocket');
            }
        }.bind(this);
        this.webSocket.onerror = function (evt) {
            if (this.debug === true) {
                console.log('openWebsocket(): ERROR: ' + evt.data);
            }
            if (this.xhrAutoFallback) {
                icsConnect.websocketIsGood = false;
                setTimeout(function () {
                    icsConnect.connectXhr();
                }, 2000);
                this.onError('Connection Error C10', 'websocket connect failed.');
            } else {
                this.onError('Connection Error C10', 'websocket connect failed.');
            }
        }.bind(this);
        // Fallback to XHR after ConnectionTimeout
        //this.connectionTimer = setTimeout(function () {
        //    if (icsConnect.webSocket != undefined) {
        //        icsConnect.webSocket.close();
        //    }
        //    if (this.xhrAutoFallback) {
        //        icsConnect.connectionMode = 'HTTP';
        //        icsConnect.connect();
        //    }
        //}, icsConnect.wsConnectionTimeout);
    },
    checkClient: function () {
        if (!window.WebSocket) {
            this.connectionMode = 'HTTP';
        }
//        if ($.browser.firefox==true && $.browser.version < 12) {
//            this.connectionMode = 'HTTP';
//        }
//        if ($.browser.safari==true && $.browser.version < 5) {
//            this.connectionMode = 'HTTP';
//        }
        if (navigator.userAgent.match(/Android/i) || navigator.userAgent.match(/OS 5/i)) {
            this.connectionMode = 'HTTP';
        }
        if (navigator.userAgent.match(/iPhone/i)) {
//            this.connectionMode = 'HTTP';
        }
    },
    disconnect: function () {
        icsConnect.appendPacket('setItem', null, null, null, 'Control.logout');
        icsConnect.transmit();
        if (icsConnect.connectionMode == 'HTTP') {
            icsConnect.onDisconnect('HTTP');
        } else {
            icsConnect.xhrAutoFallback = false;
            icsConnect.webSocket.close();
        }
        this.connected = false;
    },
    // connects by the current method
    connect: function () {
        if (!icsConnect.connectionError) {
            icsConnect.connectionError = false;
            // check connection type
            //icsConnect.checkClient();
            // if browser doesn't support WebSocket, just show some notification and exit
            if (icsConnect.connectionMode.toUpperCase() == 'HTTP') {
                icsConnect.onInitConnection('HTTP');
                icsConnect.openXhr();
            } else {
                icsConnect.onInitConnection('websocket');
                icsConnect.openWebsocket();
            }
        }
    },
    // forces XHR connection
    connectXhr: function () {
        this.connectionMode = 'HTTP';
        this.connectionError = false;
        this.connect();
    },
    getTicket: function () {
        $.ajaxSetup({async: false});
        var response = $.get(this.xhrUri + '/getTicketId');
        $.ajaxSetup({async: true});
        if (response.status !== 200) {
            if (this.debug === true) console.log("getTicket() Error: wrong status Code in return");
            return false;
        }
        icsConnect.ticket = response.responseText;
        return true;
    },
    // gets complete update of device with id=0
    getUpdate: function () {
        icsConnect.appendPacket('getUpdate');
        icsConnect.transmit();
        if (icsConnect.connectionMode == 'websocket') {
//            icsConnect.appendPacket('getItem', "*", "*", "*");// incl. channels
            icsConnect.appendPacket('getItem', "*", "*", "");// excl. channels
            icsConnect.transmit();
        }
    },
    getAllItems: function () {
        icsConnect.appendPacket('getItem', "*", "*", "", "Status.runningState");
        icsConnect.appendPacket('getItem', "*", "*", "", "Status.eventCounter");
        icsConnect.appendPacket('getItem', "*", "", "", "Status.errorCounter");
        icsConnect.transmit();
    },
    // handles incoming data
    processReceivedData: function (message) {
        if (this.compression) {
            message = $.trim(message);
            message = this.decompressMessage(message);
        }
        if (this.debug === true) {
            console.log("processReceivedData() received:");
            console.log(message.toString());
        }
        if (this.nextMessageIsResponse) {
            this.onResponse(message);
            this.nextMessageIsResponse = false;
        } else {
            this.onMessage(message);
        }
        try {
            var json = JSON.parse(message);
        }
        catch (e) {
            if (this.debug === true) {
                console.log('processReceivedData() Invalid JSON (Error C30): ', message);
                console.log(e);
            }
//            icsConnect.onInfo('Connection Error C30', 'Invalid data received');
            return false;
        }

        // Websocket Server with special wrapper object
        if (json.utf8Data !== undefined) {
            json = JSON.parse(json.utf8Data)
        }
        // checkt, ob es sich um ein Fehlerobjekt handelt
        if (icsConnect.iCSsessionId == null && (json !== undefined) &&
            (json.i !== undefined) //json.session identifier given
        ) {
            icsConnect.iCSsessionId = json.i;
            if (this.debug === true) {
                console.log('Login successful: iCSsessionID erhalten: ' + icsConnect.iCSsessionId);
            }
            icsConnect.onInfo('Connection info', 'Login successful');
        } else if ((json !== undefined) &&
            (json.c !== undefined) && //json.content
            (json.c[0].e == 'connection-error')) { //json.content.event
            icsConnect.xhrIsGood = false;
            this.connectionError = false;
            icsConnect.onError('Connection Error C21', json.c[0].d['m']); //json.content[0].data['message']
            icsConnect.disableXhrUpdates();
            if (this.xhrAutoFallback) {
                icsConnect.enableXhrUpdates(10000);
            }
            return false;
        }

        if (json instanceof Array) {
            // throw connect-Event for xhr if no error in answer package
            if (!this.xhrIsGood && this.connectionMode.toUpperCase() == 'HTTP') {
                this.xhrIsGood = true;
                // get All after 1 minute to stay up to date an to recognize connection errors in websocket mode
                icsConnect.onConnect(this.connectionMode);
            }

            // parse each Packet
            //$.each(json, function (index1, envelope) {
            _.each(json, function (envelope, index1) {
//                console.log('JAM: '+icsConnect.waitListSize);
                if ((envelope.t == "info") || (envelope.t == "response")) { //type
                    //              console.log('Pakete: '+envelope.c.length);
                    //$.each(envelope.c, function (index2, packet) {
                    _.each(envelope.c, function (packet, index2) {
                        icsConnect.waitListSize++;
                        setTimeout(function () {
                            icsConnect.processPacket(packet);
                            icsConnect.waitListSize--;
                        //}, (this.updateThreadTimeout * icsConnect.waitListSize));
                    }, (this.updateThreadTimeout * 50));
                }
            );
        }
    }
)
;
}
},
processPacket: function (packet) {
    var data = packet.d; //packet.data
    // Send notifications to connectWidget for special Items
    switch (data.i) { //data.item
        case 'Status.connectedClients':
            if (typeof icsConnectWidget != 'undefined') {
                icsConnectWidget.setClients(data.v);
            }
            icsConnect.onInfo('iCSservice Info', data.v + ' clients connected to iCSservice'); //data.value
            break;
        case 'Status.runningStateIcs':
            if (data.v == 'busy') {
                icsControl.disableInput(null, null);
                icsConnect.onInfo('iCSservice Info', 'iCS runningState update: ' + data.v);
            }
            if (data.v == 'ok') {
                icsControl.enableInput(null, null);
                icsConnect.onInfo('iCSservice Info', 'iCS runningState update: ' + data.v);
            }
            if (data.v == 'OPC_OFF') {
                icsConnect.onError('iCSservice Info', 'iCS runningState error: OPC Server offline');
            }
            if (data.v.match(/error/)) {
                var error = data.v.split('error_');
                icsConnect.onError('iCS runningState error', error[1]);
            }
            break;
        case 'Status.runningStateCAN':
            if (data.v == '00') {
//                                        icsConnect.onInfo('CAN connection ok');
            } else {
                icsConnect.onError('iCSservice Info', 'CAN connection is or was lost');
            }
            break;
        case 'Status.runningStateSystem':
            icsConnect.onInfo('iCSservice Info', 'isystem status: ' + data.v);
            break;
        case 'Status.runningStateWeb':
            icsConnect.onInfo('iCSservice Info', 'iCSwebserver status: ' + data.v);
            break;
        case 'Status.runningStateWifi':
            icsConnect.onInfo('iCSservice Info', 'iCSwifi status: ' + data.v);
            break;
        case 'Status.runningStateEthernet':
            icsConnect.onInfo('iCSservice Info', 'iCSethernet status: ' + data.v);
            break;
        case 'Status.runningStateEthernet':
            icsConnect.onInfo('iCSservice Info', 'iCSethernet status: ' + data.v);
            break;
        case 'Event.firmwareUpdateStarted':
            //data.path.device
            //data.path.module
            icsConnect.onInfo('iCSservice Info', 'Firmware-Update on device ' + data.p.l + ', Address ' + data.p.a + ' started');
            icsCommon.showFirmwareUpdateDialog();
            break;
        case 'Event.firmwareUpdateDone':
            icsConnect.onInfo('iCSservice Info', 'Firmware-Update on device ' + data.p.l + ', Module ' + data.p.a + ' done');
            icsCommon.showFirmwareUpdateDialog();
            break;
        case 'Error.firmwareUpdateFailed':
            icsConnect.onError('iCSservice Error', 'Firmware-Update on device ' + data.p.l + ', Module ' + data.p.a + ' failed');
            icsCommon.showFirmwareUpdateDialog();
            break;
        case 'Event.hardwareChanged':
            icsConnect.onError('iCSservice Error', 'Hardware Setup changed. Please check configuration using iCSconfig -> Hardware section');
            break;
        case 'Event.inputError':
            if (data.v == '1') {
                icsConnect.onInfo('Wrong input value given.');
            }
            break;
    }
    icsConnect.onRefresh(data, icsConnect.pathToId(data.p, data.i));
}
,
getWaitListSize: function () {
    return (this.waitListSize);
}
,
// extracts an path object to item-ID string
pathToId: function (path, item) {
    var id = '';
    if (path.l != undefined) {
        id += path.l + '_';
    }
    else {
        id += '-' + '_';
    }
    if (path.a != undefined) {
        id += path.a + '_';
    }
    else {
        id += '-' + '_';
    }
    if (path.c != undefined) {
        id += path.c + '_';
    }
    else {
        id += '-' + '_';
    }
    if (item != undefined) {
        id += item.replace('.', '_');
    }
    else {
        id += '-';
    }
    return id;
}
,
// appends a packet to packets array
appendPacket: function (command, line, address, channel, item, value, unit) {
    if (command == undefined) {
        var command = '';
        if (this.debug === true) {
            console.log('appendPacket(): missing command');
        }
        return false;
    }
    if (line == undefined) {
        var line = '';
    }
    if (address == undefined) {
        var address = '';
    }
    if (channel == undefined) {
        var channel = '';
    }
    if (item == undefined) {
        var item = '';
    }
    if (value == undefined) {
        var value = '';
    }
    if (unit == undefined) {
        var unit = '';
    }
    if (this.packets === undefined) {
        this.packets = [];
    }
    this.packets.push(
        {
            'c': command,
            'p': {
                'p': {'l': line, 'a': address, 'c': channel},
                'i': item,
                'v': value,
                'u': unit
            }
        }
    );
}
,
// transmits current collected packets
transmitBlock: function () {
    // in Blöcken schicken
    var blockSize = 100;
    var blockDelay = 2000; //verzögerung des Blockversands
    if (this.packets.length < blockSize) {
        var blockSize = this.packets.length;
    }
    var block = this.packets.slice(0, blockSize);
    this.packets = this.packets.slice(blockSize, this.packets.length);
    var envelope = this.getEnvelope(block);
    var message = JSON.stringify(envelope);
    var response = this.sendString(message);
    $.each(block, function (idx, packet) {
        if (packet.c == "setItem") {
            icsConnect.appendPacket('getItem', packet.p.p.l, packet.p.p.a, packet.p.p.c, packet.p.i);
        }
    });
    // wenn weitere Pakete vorhanden, dann plane erneuten transmit
    if (this.packets.length > 0) {
        setTimeout(function () {
            icsConnect.transmit();
        }, blockDelay);
    }
    return response;
}
,
// transmits current collected packets
transmit: function () {
    var envelope = this.getEnvelope(this.packets);
    var message = JSON.stringify(envelope);
    var response = this.sendString(message);
//        var sentPackets = this.packets;
    if (response !== false) {
        this.packets = [];
    }
//        var refresh = false;
//        $.each(sentPackets, function (idx, packet) {
//            if (packet.c == "setItem") {
//                refresh = true;
//                icsConnect.appendPacket('getItem', packet.p.p.l, packet.p.p.a, packet.p.p.c, packet.p.i);
//            }
//        });
//      refresh vorerst deaktiviert, da es u.U. zu kurzen Darstellungsfehlern führt....
//        if (refresh) {
//            setTimeout(function () {
//                icsConnect.transmit();
//            }, 1000);
//        }
    return response;
}
,
sendString: function (message, loginMode) {
    if (icsConnect.iCSsessionId == null && !loginMode) {
        if (this.debug === true) {
            console.log('sendString(): return false; please login to iCSservice first');
        }
        return false;
    }
    this.onSend(message);
    if (this.debug === true) {
        console.log("sendString() sent:");
        console.log(message);
    }
    if (this.compression) {
        if (this.debug) {
            var lengthUncompressed = message.toString().length;
            console.time('compressMessage');
        }
        message = this.compressMessage(message);
        var lengthCompressed = message.toString().length;
        if (this.debug) {
            console.timeEnd('compressMessage');
            console.log('sendString() Compression Rate: ' + parseFloat(lengthCompressed / lengthUncompressed).toFixed(2) + '(compr:) ' + lengthCompressed + ' to ' + lengthUncompressed + '(uncompr.)');
        }
    }
    if (this.connectionMode == 'websocket') {
        if (this.webSocket.readyState != 1) {
//                icsConnect.onopen(function () {
//                    icsConnect.sendString(message);
//                });
        }
        this.webSocket.send(message);
        this.nextMessageIsResponse = true;
    } else {
        this.nextMessageIsResponse = false;
        this.sendXhr(message);
    }
    return true;
}
,
login: function () {
    if (icsConnect.ticket == null) {
        icsConnect.getTicket();
    }
    if (icsConnect.ticket == null || icsConnect.user == null) {
        if (this.debug === true) console.log('login() Error: Login method requires ticket and user not to be null');
        return false;
    }
    var envelope = {}; // envelope
    envelope.i = '';
    envelope.t = 'login';
    envelope.c = {
        "l": icsConnect.user,
        "p": "",
        "t": icsConnect.ticket
    };
    envelope.r = this.connectionMode;// responsetype
    var message = JSON.stringify(envelope);
    var response = this.sendString(message, true);
    icsConnect.onInfo('Connection info', 'Try to login');
    if (response != true) {
        icsConnect.onError('Connection error C23 ', 'Login failed');
        if (this.debug === true) console.log('login() Error: iCS Login failed');
        return false;
    }
    return true
}
,
logout: function () {
    var envelope = {}; // envelope
    envelope.i = this.sessionId;
    envelope.t = 'logout';
    envelope.c = {};
    envelope.r = this.connectionMode;// responsetype
    var message = JSON.stringify(envelope);
    var response = this.sendString(message, true);
    icsConnect.onInfo('Connection info', 'Try to logout');
    if (response != true) {
        icsConnect.onError('Connection error C24 ', 'Logout failed');
        if (this.debug === true) console.log('logout() Error: iCS Logout failed');
        return false;
    }
    return true
}
,
// creates a new envelope wrap on a packets collection
getEnvelope: function (packets) {
    var envelope = {}; // envelope
    if (typeof(icsConnect.jsId) == "undefined") {
        // jsId should add an unique id to session id in case of multiple instances with one browser client
        icsConnect.jsId = icsCommon.getRandomKey();
    }
    envelope.i = this.iCSsessionId; //id
    envelope.t = 'request'; //type request / info
    envelope.c = packets; //content
    envelope.r = this.connectionMode;// responsetype
    return (envelope);
}
,
compressMessage: function (message) {
    if (this.huffman == null) {
        this.huffman = new Huffman();
    }
    message = this.huffman.compress(message.toString());
    message = $.base64.encode(message);
    return message;
}
,
decompressMessage: function (message) {
    var oriMessage = message;
    if (this.huffman == null) {
        this.huffman = new Huffman();
    }
    try {
        message = $.base64.decode(message);
    } catch (error) {
        console.log('Decompress Base64 error: ' + error);
        console.log('Message' + oriMessage);
        return false;
    }
    try {
        message = this.huffman.decompress(message);
    } catch (error) {
        console.log('Decompress Huffman error: ' + error);
        console.log('Message' + oriMessage);
        return false;
    }
    return message;

}
,
shutdown: function () {
    if (confirm('Do you really want to shutdown iCS ? You have to restart manually!')) {
        icsConnect.appendPacket('setItem', null, null, null, 'Control.shutdown');
        icsConnect.transmit();
        return true;
    } else {
        return false;
    }
}
,
restart: function () {
    if (confirm('Do you really want to restart iCS ? All connections will be lost')) {
        icsConnect.appendPacket('setItem', null, null, null, 'Control.reboot');
        icsConnect.transmit();
        return true;
    } else {
        return false;
    }
}
,
rescan: function () {
    if (confirm('Do you really want to rescan iCS connected devices ? Please check iCSconfig for changes.')) {
        icsConnect.appendPacket('setItem', null, null, null, 'Control.restartScan');
        icsConnect.transmit();
        return true;
    } else {
        return false;
    }
}
}
;

var icsCommon = {
 getUnixTime: function () {
    var date = new Date();
    return date.getTime() / 1000 | 0
},
getTimeString: function () {
    var MyDate = new Date();
    return ('0' + MyDate.getHours()).slice(-2) + ':'
        + ('0' + (MyDate.getMinutes() + 1)).slice(-2) + ':'
        + ('0' + (MyDate.getSeconds() + 1)).slice(-2);
},
getRandomKey: function () {
    return "0000" + (Math.random() * Math.pow(36, 4) << 0).toString(36).substr(-4);
}
};

module.exports = icsConnect;