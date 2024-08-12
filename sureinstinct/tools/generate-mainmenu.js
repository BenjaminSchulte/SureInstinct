const { createCanvas, loadImage } = require('canvas');
const path = require('path');
const fs = require('fs');

const INPUT_PATH = 'assets/other/mainmenu';
const OUTPUT_PATH = 'assets/images/mainmenu';

function getPixel(context, x, y) {
  const [r,g,b] = context.getImageData(x, y, 1, 1).data;
  return r >= 128;
}

function darken(context, x, y, index) {
  const data = context.getImageData(x, y, 1, 1);
  if (index == 0) {
    index = 1;
  }
  data.data[0] = 16 * index;
  data.data[1] = 16 * index;
  data.data[2] = 16 * index;
  data.data[3] = 255;
  context.putImageData(data, x, y)
}

async function processFrame(index, canvas, context, lastContext, target) {
  if (index == 0) {
    return;
  }

  for (var y=0; y<canvas.height; y++) {
    for (var x=0; x<canvas.width; x++) {
      const cur = getPixel(context, x, y);
      const last = getPixel(lastContext, x, y);

      if (cur && !last) {
        darken(target, x, y, index % 16)
      }
    }
  }
}

async function run() {
  var lastCanvas = null;
  var lastContext = null;

  const target = createCanvas(256, 224);
  const targetContext = target.getContext('2d');
  targetContext.clearRect(0, 0, 256, 224);

  for (var i=0; i<=16; i++) {
    const image = await loadImage(path.join(INPUT_PATH, `${(i%16).toString().padStart(4, '0')}.png`));
    const canvas = createCanvas(image.width, image.height);
    const context = canvas.getContext('2d');
    context.drawImage(image, 0, 0);

    await processFrame(i, canvas, context, lastContext, targetContext);

    lastCanvas = canvas;
    lastContext = context;
  }

  target.createPNGStream().pipe(fs.createWriteStream(path.join(OUTPUT_PATH, 'image.png')));
}

run().catch(err => console.error(err));