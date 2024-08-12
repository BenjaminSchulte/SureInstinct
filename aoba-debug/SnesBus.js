const Ram = require('./Ram');

class SnesBus {
  constructor() {
  }

  deserialize(file) {
    this.wram = Ram.fromFile(file, 0x20000);

  }
}

module.exports = SnesBus;

