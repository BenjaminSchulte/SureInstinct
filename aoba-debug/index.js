const BstFile = require('./BstFile');
const SymbolFile = require('./SymbolFile');
const Threads = require('./Threads');




if (process.argv < 3) {
  throw new Error('Missing ROM file');
}

const romFileName = process.argv[2];
const bstFileName = romFileName.substr(0, romFileName.length - 4) + '-1.bst';
const symFileName = romFileName.substr(0, romFileName.length - 4) + '.cpu.sym';

const bstFile = new BstFile(bstFileName);
const symbols = new SymbolFile(symFileName);

bstFile.update();
symbols.update();

const threads = new Threads(bstFile, symbols);

threads.update();