const { createCanvas, loadImage } = require('canvas');
const fs = require('fs');
const path = require('path');

async function run() {
  const image = await loadImage(process.argv[2]);
  const canvas = createCanvas(image.width, image.height);
  const context = canvas.getContext('2d');
  context.drawImage(image, 0, 0);

  var tileRows = [];
  var terrainRows = [];
  for (var y=0; y<image.height; y++) {
    var tileRow = [];
    var row = [];
    for (var x=0; x<image.width; x++) {
      const [r,g,b] = context.getImageData(x, y, 1, 1).data
      var terrain = -1;
      
      if (r === 255 && g === 0 && b === 0) {
        terrain = 1;
      } else if (r === 255 && g === 255 && b === 255) {
        terrain = 0;
      } else if (r === 0 && g === 0 && b === 0) {
        terrain = 2;
      }

      row.push(terrain);
      tileRow.push(0)
    }

    terrainRows.push(row.join(','));
    tileRows.push(tileRow.join(','));
  }

  var basePath = `assets/tilemaps/${process.argv[3]}/`;
  fs.writeFileSync(path.join(basePath, 'top-terrain.csv'), terrainRows.join('\n'));
  fs.writeFileSync(path.join(basePath, 'top-tiles.csv'), tileRows.join('\n'));
  fs.writeFileSync(path.join(basePath, 'bottom-terrain.csv'), terrainRows.reverse().join('\n'));
  fs.writeFileSync(path.join(basePath, 'bottom-tiles.csv'), tileRows.reverse().join('\n'));

  fs.writeFileSync(path.join(basePath, 'tilemap.yml'), `size: {width: ${image.width}, height: ${image.height}}\ntileset: wood`);
}

run().then(() => console.log('done')).catch((err) => console.error(err))
