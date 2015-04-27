var dgram = require('dgram'),
	express = require('express'),
	app = express(),
	server = require('http').Server(app),
	io = require('socket.io')(server);

server.listen(3000);

app.use(express.static(__dirname + '/public/'));

io.on('connection', function (socket) {
	socket.on('play', function (data) {
		play();
	});
	socket.on('pause', function (data) {
		pause();
	});
	socket.on('reset', function (data) {
		reset();
	});
	socket.on('max', function (data) {
		max();
	});
	socket.on('mean', function (data) {
		mean();
	});
	socket.on('min', function (data) {
		min();
	});
	socket.on('setTime', function (data) {
		setTime(data);
	});
});

// arduinos IP/PORT
var host = '192.168.2.';
var first = 2;
var last = 2;
var port = 8888;

// app receiving messages
var client = dgram.createSocket('udp4');

// default brightness
var defaultBrightness = 30;

// set default brigntness when the app starts
client.on('listening', function() {
	console.log('listening');
	for (var i = first; i <= last; i++) {
		sendText('s005', host + i);
	}
});
client.bind(3333, '192.168.2.6');

var sendText = function (text, ip) {
	var message = new Buffer(text, 'binary');
	client.send(message, 0, message.length, port, ip);
};

var play = function() {
	console.log('Play');
	for (var i = first; i <= last; i++) {
		sendText('p', host + i);
	}
};

var pause = function() {
	console.log('Pause');
	for (var i = first; i <= last; i++) {
		sendText('p', host + i);
	}
};

var reset = function() {
	console.log('Reset');
	for (var i = first; i <= last; i++) {
		sendText('t72:00:00', host + i);
	}
};

var max = function() {
	console.log('Max bright');
	for (var i = first; i <= last; i++) {
		sendText('b255', host + i);
	}
};

var mean = function() {
	console.log('Mean bright');
	for (var i = first; i <= last; i++) {
		sendText('b200', host + i);
	}
};

var min = function() {
	console.log('Min bright');
	for (var i = first; i <= last; i++) {
		sendText('b030', host + i);
	}
};

var setTime = function(time) {
	console.log('Set Time');
	for (var i = first; i <= last; i++) {
		sendText('t' + time, host + i);
	}
};