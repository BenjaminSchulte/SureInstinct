const fs = require('fs');

class FileReader {
  constructor(fileName) {
    this.buffer = fs.readFileSync(fileName);
    this.offset = 0;
  }

  readBuffer(size) {
    const buffer = this.buffer.slice(this.offset, this.offset + size - 1);
    this.offset += this.size;
    return buffer;
  }

  byte() {
    return this.buffer[this.offset++]
  }

  integer(size) {
    let value = 0;
    for (let i=0; i<size; i++) {
      value |= this.byte() << (i*8);
    }
    return value;
  }

  array(size, arraySize) {
    let arr = [];
    for (let i=0; i<arraySize; i++) {
      arr.push(this.integer(size));
    }
    return arr;
  }

  string(size) {
    const arr = this.array(1, size);
    const end = arr.indexOf(0);
    if (end !== -1) {
      arr.splice(end);
    }
    return arr.reduce((a, b) => a+String.fromCharCode(b), '');
  }
}

module.exports = FileReader;