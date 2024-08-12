const { createCanvas, loadImage } = require('canvas');
const fs = require('fs');
const path = require('path');

if (process.argv.length < 3) {
  console.log('Missing path to tileset');
  process.exit(1);
  return;
}

const name = path.basename(process.argv[2]);
const tileset = `assets/tilesets/${name}`
const imagePath = `assets/images/abyss_${name}`

async function run() {
  const tmpCanvas = createCanvas(16, 16);
  const tmpContext = tmpCanvas.getContext('2d');

  const canvas = createCanvas(16, (16*16+1)*16);
  const context = canvas.getContext('2d');
  context.clearRect(0, 0, 16, canvas.height);
  const image = await loadImage(path.join(tileset, 'main-tiles.png'));

  for (var y=0; y<16; y++) {
    for (var x=0; x<16; x++) {
      tmpContext.drawImage(image, 16, 0, 16, 16, -x, -y, 16, 16);
      tmpContext.drawImage(image, 16, 0, 16, 16,16-x, -y, 16, 16);
      tmpContext.drawImage(image, 16, 0, 16, 16,16-x, 16-y, 16, 16);
      tmpContext.drawImage(image, 16, 0, 16, 16,-x, 16-y, 16, 16);
      context.drawImage(tmpCanvas, 0, (x+y*16) * 16);
    }
  }
  
  if (!fs.existsSync(imagePath)) {
    fs.mkdirSync(imagePath)
  }

  canvas.createPNGStream().pipe(fs.createWriteStream(path.join(imagePath, 'abyss.png')));

  const content = [
    'name: Abyss',
    'color_depth: 4',
    'tile_size: {width: 8, height: 8}',
    'variants:',
    '  default:',
    '    image: abyss.png',
    `    palette: ../../tilesets/${name}/main-palette.png`
  ]

  fs.writeFileSync(path.join(imagePath, 'image.yml'), content.join('\n'));
}

run().catch(err => console.error(err))