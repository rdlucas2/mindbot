Using the makeblock starter robot kit, the mindwave mobile, and a raspberry pi to create a mind controlled robot!

The arduinoMindwave code should be uploaded to the makeblock orion arduino board.

The node app communicates between the signalr server and the robot - allowing the headset to controle the robot!

Connect the [mindbotServer](https://github.com/rdlucas2/mindbotServer) and reads the data from the bluetooth mindwave mobile headset.

Setup the raspberry pi as normal (used raspbian). Install nodejs.

To start the signalr client on raspberry pi boot, modify the rc.local file:

nano /etc/rc.local

Add the following line:

su pi -c '/usr/bin/node /home/pi/Desktop/SignalR_Client/app.js > /home/pi/Desktop/log.txt'

outputting to a log is optional - but useful for debugging.

Add a config.js file - it should look like this:

var config = {};  
config.signalrServerUrl = 'http://[SERVER IP ADDRESS HERE]:8080/signalR';  
module.exports = config;