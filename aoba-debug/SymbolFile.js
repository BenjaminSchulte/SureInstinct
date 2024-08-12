const fs = require('fs');
const { CLIENT_RENEG_WINDOW } = require('tls');

class SymbolFile {
  constructor(fileName) {
    this.fileName = fileName;

    this.symbols = {};
  }

  addr(id) {
    const sym = this.symbols[id];
    if (!sym) {
      throw new Error('No symbol named: ' + sym);
    }

    return (sym.bank << 16) | sym.address;
  }

  update() {
    const rows = fs.readFileSync(this.fileName, 'utf-8').split(/\n/g);
    let isSymbol = false;

    for (const _row of rows) {
      const row = _row.trim();
      if (!row || row[0] === '#') {
        continue;
      }

      if (row === '[SYMBOL]') {
        isSymbol = true;
      } else if (row[0] === '[') {
        isSymbol = false;
      } else if (isSymbol) {
        const bank = parseInt(row.substr(0, 2), 16);
        
        if (bank === 0x7E || bank === 0x7F) {
          const [addressStr, name] = row.split(/\s+/);
          const address = parseInt(addressStr.substr(3), 16);

          this.symbols[name] = {bank, address};
        }
      }
    }
  }
}

module.exports = SymbolFile;