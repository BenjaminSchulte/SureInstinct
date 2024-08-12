var Module = (function() {
  var romData = false;
  var isLoaded = false;
  var isDownloading = false;
  var loadProgress = 0;
  var isMouseControl = false;
  var canvas = document.getElementById('canvas');

  canvas.requestPointerLock = canvas.requestPointerLock || canvas.mozRequestPointerLock;

  function downloadFile(url, failed, success) {
    var xhr = new XMLHttpRequest(); 
    xhr.open('GET', url, true); 
    xhr.responseType = "arraybuffer";
    xhr.onreadystatechange = function () { 
      if (xhr.readyState == 4) {
        if (xhr.status === 404) {
          if (failed) failed(xhr.statusText);
        } else {
          if (success) success(xhr.response);
        }
      }
    };
    xhr.onprogress = function(evt) {
      if (!evt.lengthComputable) {
        return;
      }

      loadProgress = evt.loaded / evt.total;
      showLoadProgress(loadProgress);
    }
    xhr.send(null);
  }

  function startGame(mouse) {
    isMouseControl = mouse;

    beforeLoad();

    let myScript = document.createElement("script");
    myScript.setAttribute("src", "./snes9x-next.js");
    document.body.appendChild(myScript);

    if (!mouse) {
      document.body.className += ' keyboard';
    }
  }


  function startGameNow() {
    var mouse = isMouseControl;

    beforeStart();

    var config = [];

    // Configures the player input
    config.push('input_player1_start=enter');
    config.push('input_player1_select=shift');
    config.push('input_player1_b=s');
    config.push('input_player1_a=a');

    // Disables unwanted functions
    config.push('input_toggle_fullscreen=null');
    config.push('slowmotion_ratio=0.0');
    config.push('input_enable_hotkey_btn=null');
    config.push('input_toggle_fast_forward=null');
    config.push('input_save_state=null');
    config.push('input_load_state=null');
    config.push('input_exit_emulator=null');
    config.push('input_rate_step_up=null');
    config.push('input_rate_step_down=null');
    config.push('input_rewind=null');
    config.push('input_frame_advance=null');
    config.push('input_reset=null');
    config.push('input_cheat_toggle=null');
    config.push('input_screenshot=null');
    config.push('input_dsp_config=null');
    config.push('input_game_focus_toggle=scroll_lock');

    // Video configuration
    config.push('video_vsync = false');
    config.push('video_scale = 2.0');
    config.push('video_force_aspect = true');
    config.push('video_aspect_ratio = 1.142857');
    config.push('video_smooth = false');
    config.push('video_fullscreen_x = 512');
    config.push('video_fullscreen_y = 448');

    // Audio configuration
    config.push('audio_latency = 96');
    
    // Configures the file system
    Module.FS_createDataFile('/', "SureInstinctNTSC.sfc", new Uint8Array(romData), true, false);
    Module.FS_createFolder('/', 'etc', true, true);
    Module.FS_createDataFile('/etc', 'retroarch.cfg', config.join('\n'), true, true);

    if (mouse) {
      Module['callMain'](['-m1', "/SureInstinctNTSC.sfc"]);
    } else {
      Module['callMain'](["/SureInstinctNTSC.sfc"]);
    }
    Module.setCanvasSize(512, 448, false);

    if (mouse && canvas.requestPointerLock) {
      isMouseControl = true;
      canvas.requestPointerLock();
      canvas.addEventListener('mousedown', function() {
        canvas.requestPointerLock();
      })
    }
  }

  function updateIsLoaded() {
    if (isLoaded && romData) {
      setLoaded();
    } else {
      showLoadProgress(loadProgress);
    }
  }

  function failedDownloadRom() {
    showError('Failed to download game ROM');
  }

  function setLoadedRom(rom) {
    romData = rom;
    updateIsLoaded();
  }

  window.addEventListener('keydown', function(event) {
    if (event.key === 'F11') {
      enterFullscreen();
    }
  })

  function enterFullscreen() {
    Module.requestFullScreen(isMouseControl, false);
  }

  var Module = {
    noInitialRun: true,
    arguments: [],
    preRun: [],
    postRun: [],
    enterFullscreen: enterFullscreen,
    print: (function() {
      return function(text) {
        console.log(text);
      }
    }),
    printErr: (function() {
      return function(text) {
        showError(text);
      }
    }),
    canvas: canvas,
    startGame: startGame,
    startLoadedGame: startGameNow,
    setStatus: function(text) {
      if (Module.setStatus.interval) {
        clearInterval(Module.setStatus.interval);
      }
  
      if (text === "") {
        isLoaded = true;
        if (!isDownloading) {
          isDownloading = true;
          downloadFile('SureInstinctNTSC.sfc', failedDownloadRom, setLoadedRom);
        }
      } else {
        isLoaded = false;
      }
  
      updateIsLoaded();
    },
    totalDependencies: 0,
    monitorRunDependencies: function(left) {
      console.log(left);
    }
  };
  
  Module.setStatus('Downloading...');

  return Module;
})();
