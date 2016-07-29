//signalR server URL
var config = require('./config');

var signalrServerUrl = config.signalrServerUrl;

var client = null;
var port = null;
var connected = false;

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

wireless.on('join', function(network) {
	console.log("[    JOIN] " + network.ssid + " [" + network.address + "] ");
	setTimeout(onceConnectedToWifi, 20000);
});

wireless.on('former', function(address) {
	console.log("[OLD JOIN] " + address);
	setTimeout(onceConnectedToWifi, 20000); //20000); //use 5000 for local dev, use 20000 when starting from boot - needs some extra time for HTTP to work properly?
});

function onceConnectedToWifi() {
	if(connected) { return; }

	var SerialPort = require('serialport');
	
	port = new SerialPort('/dev/ttyUSB0', {
		parser: SerialPort.parsers.readline('\n')
	});
	
	console.log("Registering Port Events");
	port.on('error', function(err) {
		console.log('Error: ', err.message);
	});
	
	port.on('open', function() {
		setTimeout(registerHubEvents, 5000); //wait 5 seconds for the port to open, connect, and be ready for messages
	});
	
	port.on('data', function(data){
		console.log(data);
		if(data.indexOf('temp=') > -1) {
			var temp = parseInt(data.substring(5));
			if(temp > 28) {
				writePortMessage('yellow');
			}
		}
		if(data.indexOf('distance=') > -1) {
			var distance = parseInt(data.substring(9));
			if(distance < 20) {
				writePortMessage('teal');
			}
		}
	});

	var signalR = require('signalr-client');
	
	console.log("Connecting to SignalR Server...");
	client = new signalR.client(
		signalrServerUrl,
		['DataHub', 'ModeHub'] //array of hubs
		//, 10 //reconnection timeout (default is 10 seconds)
		//, false //doNotStart default is false - if true, must call client.start().
	);

	connected = true;
}

//events

var mode = '';
function registerHubEvents() {
	client.on('ModeHub', 'setMode', function(modeName) {
		console.log('********************' + modeName + '********************');
		mode = modeName;

		switch(mode) {
			case 'Circle Game':
				circleGameEvents();
				break;
			case 'Meditation':
				console.log('placeholder: register meditation events here');
				client.handlers.datahub = {
					updatedata: function(data){
						console.log(data);
						console.log('**********INSIDE MEDITATION**********');
					}
				}
				break;
			case 'Remote':
				console.log('placeholder: register Remote events here');
				client.handlers.datahub = {
					updatedata: function(data) {
						console.log(data);
						console.log('**********INSIDE REMOTE**********');
					}
				}
				break;
			case 'StopRobot':
				writeMessage('stop');
				client.handlers.datahub = {
					updatedata: function(data) {
						console.log(data);
						console.log('**********INSIDE STOPROBOT**********');
						writeMessage('stop');
					}
				}
				break;
			default:
				console.log('No mode selected!');
		}
	});

	setInterval(function() { client.invoke('ModeHub', 'PiConnected'); }, 5000);
	
	client.invoke('ModeHub', 'GetMode');
}

function circleGameEvents() {
	client.handlers.datahub = {
		updatedata: function(data) {
			console.log(data);
			var message = 'white';
			if(data.Attention > 60) {
				message = 'red';
			}
	
			if(data.Meditation > 60) {
				message = 'blue';
			}
	
			if(data.Attention > 60 && data.Meditation > 60) {
				message = 'purple';
			}
			writePortMessage(message);
		},
		getblink: function(blinkStrength) {
			console.log('Blink Strength: ', blinkStrength);
			if(blinkStrength > 50) {
				writePortMessage('green');
			}
		}
	}
}

//helper functions

function writePortMessage(message) {
	console.log(message);
	port.write(message, function(err) {
		if(err) {
			return console.log('Error on write: ', err.message);
		}
		console.log('message written');
	});	
}
