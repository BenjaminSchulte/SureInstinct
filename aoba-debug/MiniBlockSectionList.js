class MiniBlock {
  constructor(address) {
    this.address = address;
  }

  update(ram) {
    this.nextBlock = ram.uint16bankOrNull(this.address + 2);
    this.size = ram.uint16(this.address);
    this.free = false;

    if (this.size & 0x8000) {
      this.free = true;
      this.size &= 0x7FFF;
    }
  }
}

class MiniBlockSection {
  constructor(address) {
    this.address = address;
  }

  update(ram) {
    this.nextSection = ram.uint16bankOrNull(this.address);
    this.firstBlockAddress = ram.uint16bankOrNull(this.address + 2);

    this.blocks = [];
    let addr = this.firstBlockAddress;
    let max = 256;
    while (addr) {
      const block = new MiniBlock(addr);
      block.update(ram);
      addr = block.nextBlock;
      console.log(block);

      if (--max === 0) {
        throw new Error('Too many miniblocks');
      }
    }
  }
}

class MiniBlockSectionList {
  constructor(bstFile, address) {
    this.bstFile = bstFile;
    this.address = address;
  }
  
  update() {
    this.sections = [];
    if (!this.address) {
      return;
    }

    const ram = this.bstFile.bus.wram;
    let addr = this.address;
    let max = 256;
    while (addr) {
      const item = new MiniBlockSection(addr);
      item.update(ram);
      addr = item.nextSection;
      console.log(item);

      if (--max === 0) {
        throw new Error('Too many miniblock sections');
      }
    }
  }
}

module.exports = MiniBlockSectionList;