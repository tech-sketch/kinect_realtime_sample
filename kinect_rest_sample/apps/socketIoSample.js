//親クラスのappとioを受け取る
var util = require('util');
var app = module.parent.exports,  
    io = app.get('io');

io.sockets.on('connection', function(socket) {
  socket.on('send', function(msg){
    // console.log('msg : ' + util.inspect(msg,false, null));
    // 受信データを全員に送信
    io.sockets.emit('message', msg);
  });
});

