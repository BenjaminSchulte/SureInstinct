var NUM_RECORDS_FROM_CENTER = 224;

var FRAMES = [
  0,
//  0.5,
  1,
//  2,
  4,
  8,
  16,
  32,
  64,
  128,
  256,
  512,
  1024,
  4096,
]

var SUB_FRAMES = 1;

var _frames = [];
for (var i=0; i<FRAMES.length; i++) {
  var cur = FRAMES[i];
  _frames.push(cur);

  var next = FRAMES[i+1]
  if (!next || SUB_FRAMES == 0) { continue; }

  var diff = (next - cur) / (SUB_FRAMES + 1);
  for (var j=0; j<SUB_FRAMES; j++) {
    cur += diff;
    _frames.push(cur);
  }
}
FRAMES = _frames;



function buildFrame(fc) {
  const modangle = 0x0000 + fc * 0x10;
  let mod, off;

  var left = null;
  var right = null;

  var records = [];
  mod=0; off=0;
  for (var i=0; i>-NUM_RECORDS_FROM_CENTER; i--) {
    if (left === null && off < -224) {
      left = NUM_RECORDS_FROM_CENTER + i;
    }

    records[NUM_RECORDS_FROM_CENTER + i] = off ;
    mod += modangle;
    //off -= mod >> 8;
    off = -(mod >> 8);
  }

  mod=0; off=0;
  for (var i=0; i<NUM_RECORDS_FROM_CENTER; i++) {
    if (right === null && off > 224) {
      right = NUM_RECORDS_FROM_CENTER + i;
    }

    records[NUM_RECORDS_FROM_CENTER + i] = off;
    mod += modangle;
    //off += mod >> 8;
    off = +(mod >> 8);
  }

  if (left === null || right === null) {
    left = 0;
    right = NUM_RECORDS_FROM_CENTER * 2 - 1;
  }

  return {
    base: NUM_RECORDS_FROM_CENTER - left,
    records: records.slice(left, right+1)
  }
}

function printJsFrames(frames) {
  let framesJs = ['{base: -1}'];
  for (const {index, base, records} of frames) {
    if (index > 0) {
      framesJs.push(`{base: ${base}, records: [${records.join(',')}]}`);
    }
  }
  
  console.log(`var frames = [\n  ${framesJs.join(',\n  ')}\n]`)
}

function makeFmaArray(nums, prefix) {
  let i,j,chunk = 16;
  const result = [];
  for (i=0,j=nums.length; i<j; i+=chunk) {
    result.push('db ' + nums.slice(i,i+chunk).map(num => '$' + (num & 0xFF).toString(16).padStart(2, '0')).join(', '));
  }
  return prefix + result.join('\n' + prefix);
}

function printFmaFrames(frames, centerY) {
  // HEADER:
  //   db8   top_row
  //   db8   height
  //   db16  pointer (relative)

  const headerSize = (frames.length - 1) * 4;
  let header = [];
  let data = [];
  let dataOffset = headerSize;

  for (const {index, base, records} of frames) {
    if (index == 0) {
      continue;
    }
    
    let topRow = 0xFF;
    let height = 0xFF;
    let pointer = dataOffset;
    let dataSize = 0;
    var left = null;
    var right = null;
    var prev = 0;
    var isFirst = true;
    var isFirstScanline = true;

    for (var y=-112; y<112; y++) {
      var scanline = centerY + y;
      var dataIndex = base + y;
      var line = 0;

      if (dataIndex < 0) {
        line = -10000;
      } else if (dataIndex >= records.length) {
        line = 10000;
      } else {
        line = records[dataIndex];
      }

      if (line + scanline < 0) {
      } else if (line + scanline >= 224) {
        if (right === null) { right = scanline; }
        break;
      } else {
        if (isFirstScanline) {
          left = scanline;
          prev = 0;
          isFirstScanline = false;
        }

        var diff = line - prev;
        prev = line;
        if (isFirst) {
          data.push(-diff);
          isFirst = false;
        } else {
          data.push(0x100 + diff);
        }
        dataSize++;
      }
    }

    topRow = left;
    height = right - left + 1;

    header = header.concat([topRow, height, pointer, pointer >> 8]);
    dataOffset += dataSize;
  }
  console.log('module Game::Effect::Perspective');
  console.log(`  NUM_ROW_DATA_FRAMES=${frames.length-1}`);
  console.log('  datablock row_data');
  console.log('    ; Row Header');
  console.log(makeFmaArray(header, '    '));
  console.log('    ; Row Data');
  console.log(makeFmaArray(data, '    '));
  console.log('  end');
  console.log('end');
}


var frames = []
for (var frameIndex=0; frameIndex<FRAMES.length; frameIndex++) {
  frames.push({index: frameIndex, ...buildFrame(FRAMES[frameIndex])});
}

//printJsFrames(frames);
printFmaFrames(frames, 112);