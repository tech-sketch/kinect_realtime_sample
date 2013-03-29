module.exports = {

  //「/top」にリダイレクトする
  index : function(req, res){
    res.redirect('/top');
  },

  // Topページを表示する
  top : function(req, res){
    res.render('top', {title:'トップ'});
  },
};
