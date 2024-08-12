const FileReader = require('./FileReader');

const SnesBus = require('./SnesBus');

class BstFile {
  constructor(fileName) {
    this.fileName = fileName;

    this.bus = new SnesBus();
  }

  assert(a, err) {
    if (!a) throw err;
  }

  update() {
    const file = new FileReader(this.fileName);
    this.assert(file.integer(4) === 0x43545342, "File signature is invalid");
    this.assert(file.integer(4) === 15, "File version is invalid");
    file.integer(4);
    this.assert(file.string(16) === 'Compatibility', 'File profile is invalid');
    this.description = file.string(512);

    this.bus.deserialize(file);
  }
}

module.exports = BstFile;