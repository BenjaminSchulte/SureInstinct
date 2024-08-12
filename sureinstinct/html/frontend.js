var btnPlayMouse = document.getElementById('play_mouse');
var btnPlayKeyboard = document.getElementById('play_gamepad');
var canStartGame = false;

function beforeLoad() {
  document.getElementById('loading').style.display = 'block';
  document.getElementById('loaded').style.display = 'none';
}

function beforeStart() {
  document.body.className = '';
  document.getElementById('canvas').style.display = 'block';
  document.getElementById('footnote').style.display = 'block';
}

function showError(error) {
  var element = document.getElementById('error');
  element.style.display = 'block';
  element.innerText = error;

  document.getElementById('loading').style.display = 'none';
  document.getElementById('loaded').style.display = 'none';
}

function showLoadProgress(progress) {
  document.getElementById('loadinginner').style.width = Math.round(progress*100) + '%';
}

function setLoaded() {
  document.getElementById('loading').style.display = 'none';
  document.getElementById('loaded').style.display = 'none';
  document.getElementById('keyboardonly').style.display = 'inline';
  Module.startLoadedGame();
}