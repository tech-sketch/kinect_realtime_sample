var express = require('express'),
    resource = require('express-resource'), 
    pages = require('./routes/pages'), 
    http = require('http'), 
    path = require('path'),
    io = require('socket.io'),
    engine = require('ejs-locals');

// appは「connections.js」で使用するためexportsしておく
var app = module.exports = express();
var server = require('http').createServer(app);
var io = require('socket.io').listen(server);

// ioは「socketIoSample.js」で使用するためappにsetしておく
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

// 「/」でアクセスした場合は、pagesのindexを、「/top」でアクセスした場合はpagesのtopを使用する
app.get('/', pages.index);
app.get('/top', pages.top);

// express-resource用の設定。
app.resource('connections', require('./routes/connections'), {id: 'id'});

server.listen(app.get('port'), function(){
  console.log("Express server listening on port " + app.get('port'));
});
