//signalR server URL
var config = require('./config');
var os = require('os');

var signalrServerUrl = config.signalrServerUrl;

var client = null;
var port = null;

var Wireless = require('wireless');
var wireless = new Wireless({
	iface: 'wlan0',
	updateFrequency: 10,
	connectionSpyFrequency: 2,
	vanishThreshold: 2
});

wireless.enable(function(error) {
	if (error) {
		console.log("[ FAILURE] Unable to enable wireless card. Quitting...");
		return;
	}
	
	console.log("[PROGRESS] Wireless card enabled.");
	console.log("[PROGRESS] Starting wireless scan...");
	
	wireless.start();
});

/* //don't need this because we should always set up a network and connect to an existing one (use former)
wireless.on('join', function(network) {
	console.log("[    JOIN] " + network.ssid + " [" + network.address + "] ");
	setTimeout(onceConnectedToWifi, 20000);
});
*/

wireless.on('former', function(address) {
	console.log("[OLD JOIN] " + address);
	setTimeout(onceConnectedToWifi, 5000); //20000); //use 5000 for local dev, use 20000 when starting from boot - needs some extra time for HTTP to work properly?
});

function onceConnectedToWifi() {
	console.log('wifi connected');

	console.log("Connecting to SignalR Server...");
	var signalR = require('signalr-client');
	client = new signalR.client(
		signalrServerUrl,
		['DataHub', 'ModeHub'] //array of hubs
		//, 10 //reconnection timeout (default is 10 seconds)
		//, false //doNotStart default is false - if true, must call client.start().
	);
	
	console.log("Opening port and registering events");
	var SerialPort = require('serialport');
	port = new SerialPort('/dev/ttyUSB0', {
		parser: SerialPort.parsers.readline('\n')
	});
	
	port.on('error', function(err) {
		console.log('Error: ', err.message);
	});
	
	port.on('open', function() {
		console.log('port open, wait 5 seconds and register hub event');
		setTimeout(registerModeHubEvents, 5000); //wait 5 seconds for the port to open, connect, and be ready for messages
	});
	
	port.on('data', function(data){
		console.log('Message from arduino: ', data);
		/*
		if(data.indexOf('temp=') > -1) {
			var temp = parseInt(data.substring(5));
			if(temp > 28) {
				writePortMessage('yellow');
			}
		}
		if(data.indexOf('distance=') > -1) {
			var distance = parseInt(data.substring(9));
			if(distance > 0 && distance < 20) {
				writePortMessage('teal');
			}
		}
		*/
	});
}

//events

var mode = '';
function registerModeHubEvents() {
	client.on('ModeHub', 'setMode', function(modeName) {
		console.log('******************** MODE UPDATED: ' + modeName + '********************');
		mode = modeName;

		switch(mode) {
			case 'Circle Game':
				console.log('circle game mode set');
				client.handlers.datahub = {
					updatedata: function(data) {
						console.log('**********INSIDE CIRCLEGAME**********');
						console.log(data);
						var message = 'stop';

						/* // if we send these commands this quickly, not everything gets processed
						writePortMessage('attention=' + data.Attention);
						writePortMessage('meditation=' + data.Meditation);
						*/

						if(data.Attention > 60) {
							message = 'forward';
						}
				
						if(data.Meditation > 60) {
							message = 'turnLeft';
						}
						
						if(data.Attention > 60 && data.Meditation > 60) {
							message = 'circle';
						}

						writePortMessage(message);
					},
					getblink: function(blinkStrength) {
						console.log('Blink Strength: ', blinkStrength);
						if(blinkStrength > 60) {
							writePortMessage('toggleHand');
						}
					}
				}
				break;
			case 'Meditation':
				console.log('meditation mode set');
				client.handlers.datahub = {
					updatedata: function(data){
						console.log('**********INSIDE MEDITATION**********');
						console.log(data);
					},
					getblink: function(blinkStrength) {
						console.log('Blink Strength: ', blinkStrength);
					}
				}
				break;
			case 'Remote':
				console.log('remote mode set');
				client.handlers.datahub = {
					updatedata: function(data) {
						console.log('**********INSIDE REMOTE**********');
						console.log(data);
					},
					getblink: function(blinkStrength) {
						console.log('Blink Strength: ', blinkStrength);
					}
				}
				break;
			case 'StopRobot':
				console.log('STOPROBOT COMMAND SENT!');
				writePortMessage('stop');
				client.handlers.datahub = {
					updatedata: function(data) {
						console.log('**********INSIDE STOPROBOT**********');
						console.log(data);
					},
					getblink: function(blinkStrength) {
						console.log('Blink Strength: ', blinkStrength);
					}
				}
				break;
			default:
				console.log('No mode selected!');
		}
	});

	//send a message to the server every 5 seconds that the connection is still alive
	logConnectionToServer();
	
	//find out what mode is currently running on the server, to initialize the datahub events
	client.invoke('ModeHub', 'GetMode');
}

function logConnectionToServer() {
	setInterval(function() {
		var ifaces = os.networkInterfaces();
		Object.keys(ifaces).forEach(function(ifname) {
			var alias = 0;
	
			ifaces[ifname].forEach(function(iface) {
				if ('IPv4' !== iface.family || iface.internal !== false) {
					return; //skip over internal (localhost and non-ipv4 addresses)
				}
	
				if (alias >= 1) {
					//this single interface has multiple ipv4 addresses
					console.log(ifname + ':' + alias, iface.address);
					client.invoke('ModeHub', 'PiConnected', iface.address); 

				} else {
					//this interface has only one ipv4 address
					console.log('device and ip address...');
					console.log(ifname, iface.address);
					client.invoke('ModeHub', 'PiConnected', iface.address); 
				}
				++alias;
			});
		});
	}, 5000);
}

//helper functions

function writePortMessage(message) {
	console.log('sending message:', message);
	port.write(message, function(err) {
		if(err) {
			return console.log('Error on write: ', err.message);
		}
		console.log('message written');
	});	
}
