var dgram = require('dgram'),
	express = require('express'),
	app = express(),
	server = require('http').Server(app),
	io = require('socket.io')(server),
	Timer = require('./timer');


var timer = new Timer(72,0,0);
var i = 0;
var canSend = true;

timer.start();
timer.on('sec', function(time) {
	console.log(time);
	io.sockets.emit('time', time);
	setTime(time);
});
timer.on('end', function() {
	if (canSend) {
		canSend = false;
		setTimeout(function() {
			console.log('end');
			io.sockets.emit('time', '00:00:00');
			random();
			canSend = true;
		}, 10000);
	}
});
server.listen(3000);

app.use(express.static(__dirname + '/public/'));

io.on('connection', function (socket) {
	io.sockets.emit('setTime', timer.toString(true));
	socket.on('play', function (data) {
		timer.start();
	});
	socket.on('pause', function (data) {
		timer.pause();
	});
	socket.on('restart', function (data) {
		timer.restart();
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
		timer.set(data);
		io.sockets.emit('setTime', data);
	});
});

// arduinos IP/PORT
var host = '192.168.2.';
var first = 2;
var last = 4;
var port = 8888;

// app receiving messages
var client = dgram.createSocket('udp4');

// default brightness
var defaultBrightness = 30;

// set default brigntness when the app starts
client.on('listening', function() {
	console.log('listening');
	for (var i = first; i <= last; i++) {
		sendText('b255', host + i);
	}
});
client.bind(3333, '192.168.2.6');

var sendText = function (text, ip) {
	var message = new Buffer(text, 'binary');
	client.send(message, 0, message.length, port, ip);
};

var play = function() {
	console.log('Play');
	interval = setInterval(function() {

	}, 1000);
	/*for (var i = first; i <= last; i++) {
		sendText('p', host + i);
	}*/
};

var pause = function() {
	console.log('Pause');
	for (var i = first; i <= last; i++) {
		sendText('p', host + i);
	}
};

var random = function() {
	console.log('Random');
	for (var i = first; i <= last; i++) {
		sendText('r', host + i);
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