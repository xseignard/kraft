var util = require('util'),
	EventEmitter = require('events').EventEmitter;

var interval;

var Timer = function(h,m,s) {
	this.initialSec = h*3600 + m*60 + s;
	this.sec = h*3600 + m*60 + s;
};
util.inherits(Timer, EventEmitter);

Timer.prototype.start = function() {
	var self = this;
	interval = setInterval(function() {
		if (self.sec > 0) {
			self.sec--;
			self.emit('sec', self.toString());
		}
		else {
			self.emit('end');
		}
	}, 1000)
};
Timer.prototype.resume = Timer.prototype.start;

Timer.prototype.pause = function() {
	clearInterval(interval);
};

Timer.prototype.restart = function() {
	this.pause();
	this.sec = this.initialSec;
	this.start();
};

Timer.prototype.set = function(time) {
	var timeArray = time.split(':');
	this.initialSec = parseInt(timeArray[0]) * 3600 + parseInt(timeArray[1]) * 60 + parseInt(timeArray[2]);
	this.sec = parseInt(timeArray[0]) *3600 + parseInt(timeArray[1]) * 60 + parseInt(timeArray[2]);
}



Timer.prototype.toString = function(initial) {
	var timer = initial? this.initialSec : this.sec;
	var hours = Math.floor(timer / 3600);
	timer %= 3600;
	var minutes = Math.floor(timer / 60);
	var seconds = timer % 60;
	return ('0'+hours).slice(-2) +':'+ ('0'+minutes).slice(-2) +':'+ ('0'+seconds).slice(-2);
};

module.exports = Timer;