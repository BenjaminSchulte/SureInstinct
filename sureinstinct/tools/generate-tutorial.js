const {createCanvas, loadImage} = require('canvas');
const tmp = require('tmp');
const path = require('path');
const fs = require('fs');
//const yaml = require('js-yaml');
var {exec} = require('child_process');

function execAndWait(command) {
  return new Promise((resolve, reject) => {
    const child = exec(command);
    child.on('exit', (code) => {
      if (code == 0) { resolve(); return; }
      reject(new Error(`Error code ${code}`));
    });
  })
}


async function generateText(textId, text) {
  if (textId === null) {
    return;
  }

  text = text.trim();
  console.log(textId, text);

  let rowNum=0;

  const canvas = createCanvas(256, 100);
  const context = canvas.getContext('2d');

  for (const row of text.split(/\n/g)) {
    rowNum++;
    if (row === '') {rowNum-=0.5; continue;}

    const whiteImage = tmp.fileSync({postfix: '.png'});
    const blackImage = tmp.fileSync({postfix: '.png'});
    await execAndWait(`convert +antialias -background transparent -fill "#ffffff" -font "Ubuntu-Bold" -pointsize 10 "label:${row}" ${whiteImage.name}`);
    await execAndWait(`convert +antialias -background transparent -fill "#2b2b2b" -font "Ubuntu-Bold" -pointsize 10 "label:${row}" ${blackImage.name}`);

    const imageWhite = await loadImage(whiteImage.name);
    const imageBlack = await loadImage(blackImage.name);

    let x = 256 / 2 - Math.floor(imageWhite.width / 2);
    let y = (rowNum - 1) * 12;

    context.drawImage(imageBlack, x, y-1);
    context.drawImage(imageBlack, x, y+1);
    context.drawImage(imageBlack, x, y+2);
    context.drawImage(imageBlack, x-1, y-1);
    context.drawImage(imageBlack, x-1, y+1);
    context.drawImage(imageBlack, x-1, y+2);
    context.drawImage(imageBlack, x-1, y);
    context.drawImage(imageBlack, x+1, y);
    context.drawImage(imageBlack, x+1, y-1);
    context.drawImage(imageBlack, x+1, y+1);
    context.drawImage(imageBlack, x+1, y+2);
    context.drawImage(imageWhite, x, y);

    whiteImage.removeCallback();
    blackImage.removeCallback();
  }

  if (!fs.existsSync(`assets/images/tutorial-text${textId}`)) {
    fs.mkdirSync(`assets/images/tutorial-text${textId}`);
  }
  canvas.createPNGStream().pipe(fs.createWriteStream(`assets/images/tutorial-text${textId}/image.png`));
  fs.writeFileSync(`assets/images/tutorial-text${textId}/image.yml`, [
    `name: Tutorial text ${textId}`,
    '',
    'color_depth: 4',
    'tile_size: {width: 8, height: 8}',
    'relative_tile_index: 64',
    '',
    'variants:',
    '  default:',
    '    image: image.png',
    '    priority: ../user_interface/priority.png',
    '    palette: ../user_interface/palette.png',
  ].join('\n'))
}

async function run() {
  let textId = null;
  let text;

  for (const row of fs.readFileSync('assets/levels/tutorial/messages.txt', 'utf-8').split(/\n/g)) {
    if (row[0] === '#') {
      await generateText(textId, text);
      textId = parseInt(row.substr(1), 10);
      text = '';
      continue;
    }

    text += row + '\n';
  }
  await generateText(textId, text);
}

run().then(() => console.log('done')).catch(function(err) {
  console.error(err);
})