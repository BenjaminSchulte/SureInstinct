const MiniBlockSectionList = require('./MiniBlockSectionList');

class Thread {
  constructor(address) {
    this.address = address;
  }

  update(ram, active) {
    this.nextThread = ram.uint16bankOrNull(this.address);

    this.id = ram.uint8(this.address + 2);
    this.parentId = ram.uint8(this.address + 3);
    this.status = ram.uint8(this.address + 4);
    this.eventMask = ram.uint8(this.address + 5);
    this.priority = ram.uint8(this.address + 6);
    this.stackAddress = ram.uint16(this.address + 7);
    this.firstEventAddress = ram.uint16bankOrNull(this.address + 9);
    this.lastEventAddress = ram.uint16bankOrNull(this.address + 11);
    this.miniBlockSectionAddress = ram.uint16bankOrNull(this.address + 13);
  }
}

class Threads {
  constructor(bstFile, symbols) {
    this.bstFile = bstFile;
    this.symbols = symbols;
  }

  update() {
    this.queuedThreads = this.fetch('.queued_threads', true);
    this.pausedThreads = this.fetch('.paused_threads', true);
  }

  fetch(symbol, active) {
    const ram = this.bstFile.bus.wram;
    let addr = ram.uint16bankOrNull(this.symbols.addr(symbol));
    let list = [];
    let max = 32;

    while (addr) {
      const thread = this.fetchOne(ram, addr, active);
      addr = thread.nextThread;
      list.push(thread);

      if (--max === 0) {
        throw new Error('Too many threads for ' + symbol);
      }
    }

  
for (const item of list) {
  console.log(`\x1b[32;1m${symbol} THREAD ${item.address.toString(16).padStart(6, '0')}\x1b[m`)
  console.log(item);

  const sections = new MiniBlockSectionList(this.bstFile, item.miniBlockSectionAddress);
  sections.update();
}

    return list;
  }

  fetchOne(ram, addr, active) {
    const thread = new Thread(addr);
    thread.update(ram, active);
    return thread;
  }
}

module.exports = Threads;