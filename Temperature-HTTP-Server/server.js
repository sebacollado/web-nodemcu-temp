/**************************************
 *             DEPENDENCIES
 **************************************/
var fs = require('fs');
var ssl = true;
var credentials = {key: fs.readFileSync('sslcert/key.pem', 'utf8'), cert: fs.readFileSync('sslcert/cert.pem', 'utf8')};

var express = require('express');
var app = express();
//var server = (ssl) ? require('https').Server(app) : require('http').Server(app);
var server = (ssl) ? require('https').Server(credentials, app) : require('http').Server(app);
var io = require('socket.io')(server);

var bodyParser = require('body-parser');
var path = require('path');

app.use(bodyParser.json()); // support json encoded bodies
app.use(bodyParser.urlencoded({ extended: true })); // support encoded bodies
app.use(express.static(__dirname + '/public/assets'));

/**************************************
 *           SERVER STORAGE
 **************************************/
var jsonarray = [];

/**************************************
 *             WEB SERVER
 **************************************/

// Static files
app.get('/', function (req, res) {
    res.sendFile(path.join(__dirname, '/public/views/index.html'));
});

/**
 * POST new temperature value
 * @param json {key="", temp=""}
 */
app.post('/temperature', function (req, res) {
    console.log(req.body);
    // Now datetime
    var datetime = Date.now();
    // Check if parameters exists
    if(req.body.temp && req.body.key){
        // Broadcast to clients
        jsonarray.push({key: req.body.key, temp: req.body.temp, datetime: datetime});
        io.emit('json temp', jsonarray);
        
        res.send(req.body);
    }else{
        console.log("Bad parameters");
        res.send(req.body);
    }

    if(jsonarray.length > 60) jsonarray.splice(0,1);
});

// Change the 404 message modifing the middleware
app.use(function(req, res, next) {
    res.sendFile(path.join(__dirname, '/public/views/404.html'));
});

/**************************************
 *             WEBSOCKET
 **************************************/
io.on('connection', function(socket) {
    io.emit('json temp', jsonarray);
	console.log('Client connected');
});

/**************************************
 *             SERVER START
 **************************************/
var port = ssl ? 443 : 80;
server.listen(port, function () {
    console.log('Server running on port ' + port);
});