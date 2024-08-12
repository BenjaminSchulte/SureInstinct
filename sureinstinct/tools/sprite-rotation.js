const {createCanvas, loadImage} = require('canvas');
const path = require('path');
const fs = require('fs');

const spritePath = process.argv[2];
if (!spritePath) {
  throw new Error(`Missing sprite path`);
}

const numAngles = 32;
const startAngle = 0;
const endAngle = Math.PI * 2;

async function run() {
  const image = await loadImage(path.join(spritePath, 'input.png'));
  const width = image.width;
  const height = image.height;
  const pivotX = width / 2;
  const pivotY = height / 2;
  const imageCsv = ['image,palette,x,y,w,h,px,py,priority,include']

  const canvasList = [];
  const stepSize = (endAngle - startAngle) / (numAngles);
  var angle = startAngle;
  for (var i=0; i<numAngles; i++) {
    const canvas = createCanvas(width, height);
    const context = canvas.getContext('2d');
    context.clearRect(0, 0, width, height);
    context.imageSmoothingEnabled = false;

    context.translate(pivotX, pivotY);
    context.rotate(angle);
    context.translate(-pivotX, -pivotY);
    context.drawImage(image, 0, 0);

    canvasList.push(canvas);
    angle += stepSize;
  }


  const fullCanvas = createCanvas(width, height * numAngles);
  const fullContext = fullCanvas.getContext('2d');
  fullContext.clearRect(0, 0, width, height * numAngles);
  
  let targetY = 0;
  for (const item of canvasList) {
    fullContext.drawImage(item, 0, targetY);
    imageCsv.push(`main-frames.png,main-palette.png,0,${targetY},${width},${height},${Math.round(pivotX)},${Math.round(pivotY)},0,`)

    targetY += height;
  }

  fullCanvas.createPNGStream().pipe(fs.createWriteStream(path.join(spritePath, 'main-frames.png')));
  fs.writeFileSync(path.join(spritePath, 'images.csv'), imageCsv.join('\n'))
}

run().then(function() {console.log('Done')}).catch(err => console.error(err));