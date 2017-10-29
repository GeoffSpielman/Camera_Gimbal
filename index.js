var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var socket = io.listen(http);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

app.get('/CameraInterface.css', function(req, res){
  res.sendfile('CameraInterface.css');
});


io.on('connection', function(socket){
	console.log('a user connected');
	var client_ip_address = socket.request.connection.remoteAddress;
	console.log('New connection from ' + client_ip_address);
	
	var SerialPort = require('serialport');
	var serialport = new SerialPort('/dev/ttyACM0');
	serialport.on('open', function(){
		console.log('Connected to Arduino via Serial Port');

		serialport.on('data', function(data){
		      console.log(data);
	  });
	});

	socket.on('disconnect', function(){
    		console.log('user disconnected');
	});

	socket.on('debugMessage', function(msg){
    		console.log('Debug Message: ' + msg);
		io.emit('debugMessage', msg)
  	});

	socket.on('cameraMessage', function(msg){
		console.log('Camera Message: ' + msg);
		serialport.write(msg + '~');				 
	});
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});