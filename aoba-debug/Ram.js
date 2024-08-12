class Ram {
  constructor(buffer) {
    this.buffer = buffer;
  }

  static fromFile(file, size) {
    return new Ram(file.readBuffer(size));
  }

  uint8(addr) {
    return this.buffer[this.toAddr(addr)];
  }

  uint16(addr) {
    return this.uint8(addr) | (this.uint8(addr + 1) << 8);
  }

  uint16bank(addr) {
    return this.uint16(addr) | (addr & 0xFF0000);
  }

  uint16bankOrNull(addr) {
    const value = this.uint16(addr);
    if (value === 0) {
      return null;
    }
    
    return value | (addr & 0xFF0000);
  }

  toAddr(addr) {
    const isAddr = addr - 0x7E0000;
    if (isAddr < 0 || isAddr >= this.buffer.length) {
      throw new Error('Out of RAM range');
    }
    return isAddr;
  }
}

module.exports = Ram;