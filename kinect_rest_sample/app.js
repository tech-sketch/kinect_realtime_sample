var express = require('express'),
    resource = require('express-resource'), 
    pages = require('./routes/pages'), 
    http = require('http'), 
    path = require('path'),
    io = require('socket.io'),
    engine = require('ejs-locals');

// app�́uconnections.js�v�Ŏg�p���邽��exports���Ă���
var app = module.exports = express();
var server = require('http').createServer(app);
var io = require('socket.io').listen(server);

// io�́usocketIoSample.js�v�Ŏg�p���邽��app��set���Ă���
app.set('io', io);
app.engine('ejs', engine);

app.configure(function(){
  app.set('port', process.env.PORT || 8888);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'ejs');
  app.use(express.favicon());
  app.use(express.logger('dev'));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(express.cookieParser('your secret here'));
  app.use(express.session());
  app.use(app.router);
  app.use(express.static(path.join(__dirname, 'public')));
});

app.configure('development', function(){
  app.use(express.errorHandler());
});

// �u/�v�ŃA�N�Z�X�����ꍇ�́Apages��index���A�u/top�v�ŃA�N�Z�X�����ꍇ��pages��top���g�p����
app.get('/', pages.index);
app.get('/top', pages.top);

// express-resource�p�̐ݒ�B
app.resource('connections', require('./routes/connections'), {id: 'id'});

server.listen(app.get('port'), function(){
  console.log("Express server listening on port " + app.get('port'));
});
