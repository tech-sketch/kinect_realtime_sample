// app.jsでexportsしたappを代入し、/socketIoSampleで使えるように再度exportsする。
var app = module.exports = module.parent.exports,
    io = app.get('io');

//socket.ioを使うための準備
app.set('io', io);
require('../apps/socketIoSample');

module.exports = {
  index: function(req, res) {
    console.log('index');
    res.send(connection);
  },
  new: function(req, res) {
    res.send(405, "Method Not Allowed");
  },
  create: function(req, res) {
    //postリクエストが来たときにconnectionのデータをクライアント側
    //に送信する
//    var util = require('util');
//    console.log('req.body : ' + util.inspect(req.body),false, null);
//    console.log('req.body.connection : ' +  req.body.connection);

    var jcon = JSON.parse(req.body.connection);
//    console.log('req.body.nakami : ' + jcon.data[1].count);
    io.sockets.emit('message',jcon);
    res.send(200, "create");
  },
  show: function(req, res) {

        res.send(405, "Method Not Allowed");
  },
  edit: function(req, res) {
    res.send(405, "Method Not Allowed");
  },
  update: function(req, res) {
    res.send(405, "Method Not Allowed");
  },
  destroy: function(req, res) {
    res.send(405, "Method Not Allowed");
  }
};

var connection = {
    "max": '90',
    "data": [{}]
};
