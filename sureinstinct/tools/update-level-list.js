const {createCanvas} = require('canvas');
const tmp = require('tmp');
const path = require('path');
const fs = require('fs');
const yaml = require('js-yaml');
var {exec} = require('child_process');

const screenshotSize = {x: 80, y: 80}

const colors = {
  bg: '#1f1f1f',
  floor: '#e7e5cb',
  wall: '#ff5246',
  ice: '#5dbfa9',
  danger: '#5dbf7b',
  goal: '#ffffff',
  extra: '#8f51a2',
  logic: '#4463a1',
  arrow: '#31979f'
}

const terrainToColor = {
  '-1': colors.bg,
  '0': colors.floor,
  '1': colors.wall,
  '2': colors.bg, //abyss
  '3': colors.ice
}

const tileToColor = {
  '1': colors.bg,
  '8': colors.wall,
  '9': colors.wall,
  '10': colors.wall,
  '11': colors.wall,
  '12': colors.wall,
  '13': colors.wall,
  '14': colors.wall,
  '15': colors.wall,
  '16': colors.wall,
  '17': colors.wall,
  '18': colors.wall,
  '19': colors.wall,
  '20': colors.wall,
  '21': colors.wall,
  '22': colors.wall,
  '23': colors.wall,
  '24': colors.ice,
  '25': colors.ice,
  '26': colors.arrow,
  '27': colors.arrow,
  '28': colors.arrow,
  '29': colors.arrow,
  '40': colors.extra,
  '41': colors.extra,
  '42': colors.extra,
  '43': colors.extra,
  '44': colors.extra,
  '45': colors.extra,
  '46': colors.extra,
  '47': colors.extra,
  '48': colors.logic,
  '49': colors.logic,
  '50': colors.logic,
  '51': colors.logic,
  '52': colors.logic,
  '53': colors.logic,
  '54': colors.logic,
  '55': colors.logic,
  '56': colors.goal,
  '57': colors.goal,
  '58': colors.goal,
  '59': colors.goal,
  '60': colors.goal,
  '64': colors.danger,
  '65': colors.danger,
  '66': colors.danger,
  '67': colors.danger,
  '68': colors.wall,
  '69': colors.wall,
  '70': colors.wall,
  '71': colors.wall,
  '72': colors.extra,
  '73': colors.extra,
  '74': colors.extra,
  '75': colors.extra,
  '96': colors.wall,
  '97': colors.wall,
  '98': colors.wall,
  '96': colors.wall,
  '101': colors.wall,
  '102': colors.wall,

}



function execAndWait(command) {
  return new Promise((resolve, reject) => {
    const child = exec(command);
    child.on('exit', (code) => {
      if (code === 0) { resolve(); }
      reject(`Error code ${code}`)
    });
  })
}

async function screenshot(stage) {
  const terrain = fs.readFileSync(`assets/tilemaps/${stage}/top-terrain.csv`, 'utf8').split(/\r?\n/g).map(row => row.trim().split(/,/g));
  const tiles = fs.readFileSync(`assets/tilemaps/${stage}/top-tiles.csv`, 'utf8').split(/\r?\n/g).map(row => row.trim().split(/,/g));
  const height = terrain.length;
  const width = terrain[0].length;

  let alignX = 0;//(screenshotSize.x - 1 - width) >> 1;
  let alignY = 0;//(screenshotSize.y - 1 - height) >> 1;

  const canvas = createCanvas(width, height);
  const context = canvas.getContext('2d');
  context.fillStyle = colors.bg;
  context.fillRect(0, 0, width, height);
  const data = context.getImageData(0, 0, width - 1, height - 1);

  var top = 128;
  var bottom = 0;
  var left = 128;
  var right = 0;

  for (var y=0; y<height; y++) {
    const ty = y + alignY;
    if (ty < 0 || ty >= height - 1) {
      continue;
    }

    for (var x=0; x<width; x++) {
      const tx = x + alignX;
      if (tx < 0 || tx >= width - 1) {
        continue;
      }

      const index = (tx + ty * (width - 1)) * 4;

      const terTile = parseInt(terrain[y][x], 10).toString();
      const tile = parseInt(tiles[y][x], 10).toString();
      
      let color = terrainToColor[terTile];
      if (tileToColor.hasOwnProperty(tile)) {
        color = tileToColor[tile];
      }

      if (color !== colors.bg) {
        left = Math.min(left, x);
        right = Math.max(right, x);
        top = Math.min(top, y);
        bottom = Math.max(bottom, y);
      }

      const [, rh, gh, bh] = color.match(/^#([a-f0-9A-F]{2})([a-f0-9A-F]{2})([a-f0-9A-F]{2})$/);
      const r = parseInt(rh, 16);
      const g = parseInt(gh, 16);
      const b = parseInt(bh, 16);
      data.data[index + 0] = r;
      data.data[index + 1] = g;
      data.data[index + 2] = b;
    }
  }

  context.putImageData(data, 0, 0);

  const targetDir = `assets/images/map_${stage}`;
  if (!fs.existsSync(targetDir)) {
    fs.mkdirSync(targetDir)
  }

  const outCanvas = createCanvas(screenshotSize.x, screenshotSize.y);
  const outContext = outCanvas.getContext('2d');
  outContext.antialias = false;
  outContext.imageSmoothingEnabled = false;
  outContext.fillStyle = colors.bg;
  outContext.fillRect(0, 0, screenshotSize.x, screenshotSize.y);

  const actualWidth = right - left + 1;
  const actualHeight = bottom - top + 1;

  var target = {x: 0, y: 0, w: screenshotSize.x, h: screenshotSize.y};
  var source = {x: 0, y: 0, w: actualWidth, h: actualHeight};

  if (actualWidth < screenshotSize.x) {
    target.x += (screenshotSize.x - actualWidth) >> 1;
    target.w = actualWidth;
  } else if (actualWidth > screenshotSize.x) {
    let diff = actualWidth - screenshotSize.x;
//    console.log(stage, 'SMALLER X', diff)
    //source.x += diff >> 1;
    //source.w = screenshotSize.x;
  }
  if (actualHeight < screenshotSize.y) {
    target.y += (screenshotSize.y - actualHeight) >> 1;
    target.h = actualHeight;
  } else if (actualHeight > screenshotSize.y) {
    let diff = screenshotSize.y - actualHeight;
    source.y += diff >> 1;
    source.h = screenshotSize.y;
  }

  source.x += left;
  source.y += top;

  outContext.drawImage(canvas, source.x, source.y, source.w, source.h, target.x, target.y, target.w, target.h);

  outCanvas.createPNGStream().pipe(fs.createWriteStream(path.join(targetDir, 'image.png')));


  fs.writeFileSync(path.join(targetDir, 'image.yml'), [
    'name: Stage screenshot ' + stage,
    'color_depth: 16',
    'tile_size: {width: 8, height: 8}',
    'relative_tile_index: 704',
    'variants:',
    '  default: {image: image.png, priority: ../stageselect/priority0.png, palette: ../stageselect/bg1-palette.png}'
  ].join('\n'))
}

async function createStageName(stage, name, selectedImage, image) {
  const stageImage = tmp.fileSync({postfix: '.png'});
  const nameImage = tmp.fileSync({postfix: '.png'});

  await execAndWait(`convert +antialias -background transparent -fill "#ffffff" -font "Ubuntu-Bold" -pointsize 10 "label:${stage}" ${stageImage.name}`);
  await execAndWait(`convert +antialias -background transparent -fill "#020100" -font "Ubuntu-Light" -pointsize 8 "label:${name}" ${nameImage.name}`);
  await execAndWait(`convert -size 112x32 xc:transparent -draw "image over 0,0 0,0 'assets/images/stageselect/stage_active_template.png'" -draw "image over 8,5 0,0 '${stageImage.name}'" -draw "image over 8,14 0,0 '${nameImage.name}'" ${selectedImage}`);

  await execAndWait(`convert +antialias -background transparent -fill "#ffac48" -font "Ubuntu-Bold" -pointsize 10 "label:${stage}" ${stageImage.name}`);
  await execAndWait(`convert +antialias -background transparent -fill "#020100" -font "Ubuntu-Light" -pointsize 8 "label:${name}" ${nameImage.name}`);
  await execAndWait(`convert -size 112x32 xc:transparent -draw "image over 0,0 0,0 'assets/images/stageselect/stage_template.png'" -draw "image over 8,5 0,0 '${stageImage.name}'" -draw "image over 8,14 0,0 '${nameImage.name}'" ${image}`);

  stageImage.removeCallback();
  nameImage.removeCallback();
}

const stageList = [];

async function loadStage(id) {
  const doc = yaml.load(fs.readFileSync(`assets/levels/${id}/level.yml`, 'utf-8'));
  if (!doc.custom_properties || !doc.custom_properties.name || !doc.custom_properties.stage || doc.custom_properties.stage_index === undefined) {
    console.warn(`Skipping level ${id}`);
    return;
  }
  
  const stage = doc.custom_properties.stage;
  const name = doc.custom_properties.name;
  const index = doc.custom_properties.stage_index;
  if (stageList[index] !== undefined) {
    console.warn(`Stage index used twice ${index}`);
    return;
  }

  const variantId = `name_${id}`
  await createStageName(stage, name, `assets/images/stageselect/${variantId}_selected.png`, `assets/images/stageselect/${variantId}.png`);
  await screenshot(id);

  stageList[index] = {stage, name, variantId};
}

async function run() {
  const dirs = fs.readdirSync('assets/levels');
  await Promise.all(dirs.map(dir => loadStage(dir)));
  //await Promise.all(stageList.map(data => data ? null : createStage(data)));
}

run().then(() => console.log('Done.')).catch(err => console.error(err));
