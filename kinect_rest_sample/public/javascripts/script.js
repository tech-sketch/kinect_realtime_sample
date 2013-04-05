window.addEventListener('load', function() {

  load();
  var socket = io.connect('http://example.com:8888/');
  // 受信したmsgをtextareaに表示
  socket.on('message', function(msg){
    data = msg;
    draw();
  });
}, false);

function load() {
  var canvas = document.getElementById("cvs1");
  var ctx = canvas.getContext('2d');

  /* Imageオブジェクトを生成 */
  var img = new Image();
  img.src = "/images/images.png";

  /* 画像を描画 */
    img.onload = function() {
      ctx.drawImage(img, 0, 0);
      draw();
  };
}

function draw(){
    var config = {
        element: document.getElementById("heatmapArea"),
        radius: 20,
        opacity: 1000
    };

    //creates and initializes the heatmap
    var heatmap = h337.create(config);
    console.log(data);
    heatmap.store.setDataSet(data);
}
