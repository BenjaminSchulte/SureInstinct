const fs = require('fs');

let maxClock = 0;

class DataReader {
  constructor(fileName) {
    this.buffer = fs.readFileSync(fileName);
    this.index = 0;
    this.clockOffset = 0;
  }

  read(len) {
    if (this.index + len > this.buffer.length) {
      return -1;
    }
    let value = this.buffer[this.index++];
    let shift = 8;
    while (--len) {
      value |= this.buffer[this.index++] << shift;
      shift += 8;
    }
    return value;
  }

  byte() {
    return this.read(1);
  }

  word() {
    return this.read(2);
  }

  withNewClock() {
    const clock = this.read(4);
    let duration;
    if (clock < this.clockOffset) {
      duration = clock + (0x100000000 - this.clockOffset);
    } else {
      duration = clock - this.clockOffset;
    }
    this.clockOffset = clock;
    maxClock = Math.max(maxClock, duration);
    return duration;
  }

  clock() {
    const clock = this.read(4);
    let duration;
    if (clock < this.clockOffset) {
      duration = clock + (0x100000000 - this.clockOffset);
    } else {
      duration = clock - this.clockOffset;
    }
    maxClock = Math.max(maxClock, duration);
    return duration;
  }
}

class ThreadIdentify {
  constructor(name, events) {
    this.name = name;
    this.events = events || {};
  }
}

let internalId = 1;
const threads = {}
const allThreads = [];
class ThreadInstance {
  constructor(id) {
    this.id = id;
    this.internalId = internalId++;
    this.type = new ThreadIdentify('unknown');
    this.events = [];
    allThreads.push(this);
  }

  identifyAs(id) {
    switch (id) {
      case 1: this.type = new ThreadIdentify(`audio`); break;
      case 2: this.type = new ThreadIdentify(`level`); break;
      case 3: this.type = new ThreadIdentify(`palette`); break;
      case 4: this.type = new ThreadIdentify(`script`); break;
      case 5: this.type = new ThreadIdentify(`sprite`); break;
      case 6: this.type = new ThreadIdentify(`tileset`); break;
      case 7: this.type = new ThreadIdentify(`font`); break;
      case 8: this.type = new ThreadIdentify(`ingamescene`); break;
      case 9: this.type = new ThreadIdentify(`ingamecomp`); break;
      default: this.type = new ThreadIdentify(`thread${id}`); break;
    }
  }

  static create(id) {
    threads[id] = new ThreadInstance(id);
    return threads[id];
  }

  enterEvent(id) {
    this.events.push(id);
  }

  leaveEvent() {
    this.events.pop();
  }
}

class FrameEvent {
  constructor(clock) {
    this.clock = clock;
  }

  toJson() {
    throw new Error('Missing toJson');
  }
}

class ThreadEvent extends FrameEvent {
  constructor(clock, thread) {
    super(clock);
    this.thread = thread;
  }

  toJson() {
    return {type: 'thread', clock: this.clock, thread: this.thread.internalId};
  }
}

class StartEventEvent extends FrameEvent {
  constructor(clock, thread, event) {
    super(clock);
    this.thread = thread;
    this.event = event;
  }

  toJson() {
    return {type: 'event-start', clock: this.clock, thread: this.thread.internalId, event: this.event}
  }
}

class EndEventEvent extends FrameEvent {
  constructor(clock, thread) {
    super(clock);
    this.thread = thread;
  }

  toJson() {
    return {type: 'event-stop', clock: this.clock, thread: this.thread.internalId}
  }
}

class IdleEvent extends FrameEvent {
  toJson() {
    return {type: 'idle', clock: this.clock}
  }
}

class Frame {
  constructor(start, ramUsage, miniRamUsage, id) {
    if (id === undefined) {
      throw new Error('missing id');
    }

    this.id = id;
    this.start = start;
    this.events = []
    this.thread = null;
    this.ramUsage = ramUsage;
    this.miniRamUsage = miniRamUsage;
  }

  get threadId() {
    if (this.thread === null) {
      return -1;
    }

    return this.thread.id;
  }

  get threadType() {
    if (this.thread === null) {
      return 'system';
    }

    return this.thread.type.name;
  }

  gotoThread(clock, id) {
    if (!threads[id]) {
      console.warn('Entered unknown thread ID ' + id);
      threads[id] = new ThreadInstance(id);
    }

    this.events.push(new ThreadEvent(clock, threads[id]))
    this.thread = threads[id];
  }

  enterEvent(clock, id) {
    this.thread.enterEvent(id);
    this.events.push(new StartEventEvent(clock, this.thread, id));
  }

  leaveEvent(clock) {
    this.thread.leaveEvent();
    this.events.push(new EndEventEvent(clock, this.thread));
  }

  startIdle(clock) {
    this.events.push(new IdleEvent(clock));
  }
};


function dumpThreadName(thread) {
  if (thread) {
    return `${thread.id.toString(16).padEnd(2)} ${thread.type.name}`;
  } else {
    return '   system';
  }
}

function dumpUsage(usage) {
  console.log(` Frame | Thread        |  Addr | Size`);
  console.log('-------+---------------+-------+------');

  for (const {size, miniram, address, thread, frame} of Object.values(usage).sort((a, b) => a.frame - b.frame)) {
    const row = [];
    let prefix = miniram ? "+" : ' ';
    row.push(frame.toString().padStart(6));
    row.push(dumpThreadName(thread).padEnd(13));
    row.push(prefix + address.toString(16).padStart(4, '0'));
    row.push(size.toString(16).padStart(4, '0'));
    //console.log(address);

    console.log(row.join(' | '))
  }
}

function freeThreadRam(usage, threadId) {
  let remove = [];
  for (const {address, thread} of Object.values(usage)) {
    if (thread && thread.id === threadId) {
      remove.push(address);
    }
  }

  for (const item of remove) {
    delete usage[item]
  }
}

function dumpEventUsage(type, usage) {
  for (const {address, thread, frame} of Object.values(usage)) {
    if (thread === false) {
      continue;
    }

    console.log(`${type} at ${address.toString(16)} by thread ${dumpThreadName(thread).trim()} in frame ${frame} not freed`)
  }
}

const frames = []
const miniRamUsageByAddress = {};
const ramUsageByAddress = {}
const threadEvents = {};
let frame = new Frame(-1, 0, 0, 0);


const data = new DataReader('./bsnes_debug.out');
do {
  const command = data.byte();
  if (command === -1) {
    break;
  }

  const error = `FRAME ${frame.id}: `;

  switch (command) {
    case 1: // IRQ
      frame = new Frame(data.withNewClock(), frame.ramUsage, frame.miniRamUsage, frame.id+1);
      frames.push(frame);
      break;

    case 2: // IDLE
      frame.startIdle(data.clock());
      break;
    
    case 3: // THREAD ENTER
      frame.gotoThread(data.clock(), data.byte());
      break;
    
    case 4: // EVENT START
      frame.enterEvent(data.clock(), data.byte());
      break;

    case 5: // STOP EVENT
      frame.leaveEvent(data.clock());
      break;

    case 6: {// THREAD INTRODUCE
      const id = data.byte();
      threads[id].identifyAs(data.byte());
      break;
    }
    
    case 7: // THREAD CREATE
      ThreadInstance.create(data.byte());
      break;

    case 8: {// ALLOCATE RAM
      let address = data.word();
      let size = data.word();
      ramUsageByAddress[address] = {size, address, frame: frame.id, thread: frame.thread};
      frame.ramUsage += size;
      break;
    }

    case 9: {// FREE RAM
      let address = data.word();
      let size = data.word();
      size = (size + 0xFF) & 0xFF00;
      if (!ramUsageByAddress.hasOwnProperty(address)) {
        console.warn(`${error} Warning: RAM address ${address.toString(16)} has been freed twice`);
      } else if (size !== ramUsageByAddress[address].size) {
        console.warn(`${error} Warning: RAM address ${address.toString(16)} has been freed with invalid size: ${size.toString(16)}`);
      } else {
        delete ramUsageByAddress[address];
      }
      frame.ramUsage -= size;
      break;
    }

    case 10: {// ALLOCATE MINIRAM
      let address = data.word();
      let size = data.word();
      miniRamUsageByAddress[address] = {size, address, frame: frame.id, thread: frame.thread, miniram: true};
      frame.miniRamUsage += size;
      break;
    }

    case 11: {// FREE MINIRAM
      let address = data.word();
      let size = 0;
      if (!miniRamUsageByAddress.hasOwnProperty(address)) {
        console.warn(`${error} Warning: MiniRAM address ${address.toString(16)} has been freed twice`);
      } else {
        size = miniRamUsageByAddress[address].size;
        delete miniRamUsageByAddress[address];
      }
      frame.miniRamUsage -= size;
      break;
    }

    case 12: {// END OF THREAD
      let thread = data.byte();
      freeThreadRam(ramUsageByAddress, thread);
      freeThreadRam(miniRamUsageByAddress, thread);
      break;
    }

    case 13: { // THREAD EVENT ALLOCATE
      let address = data.word();
      if (!threadEvents[address]) {
        console.warn(`${error} Allocated invalid thread event address ${address}`)
      } else if (threadEvents[address].thread !== false) {
        console.warn(`${error} Allocated thread event that is already allocated`);
      } else {
        threadEvents[address].frame = frame.id;
        threadEvents[address].thread = frame.thread;
      }
      break;
    }

    case 14: { // THREAD EVENT DESTROY
      let address = data.word();
      if (!threadEvents[address]) {
        console.warn(`${error} Freed invalid thread event address ${address}`)
      } else if (threadEvents[address].thread === false) {
        console.warn(`${error} Freed thread event that is already freed`);
      } else {
        threadEvents[address].thread = false;
      }
      break;
    }

    case 15: { // THREAD EVENT DISPATCH
      let address = data.word();
      if (!threadEvents[address]) {
        console.warn(`${error} Freed invalid thread event address ${address}`)
      } else if (threadEvents[address].thread === false) {
        console.warn(`${error} Freed thread event that is already freed`);
      } else {
        threadEvents[address].thread = false;
      }
      break;
    }

    case 16: { // REGISTER THREAD EVENT
      let address = data.word();
      threadEvents[address] = {address, thread: false};
      break;
    }

    default:
      throw new Error(`Unsupported command ${command}`);
  }
} while(true);



const json = {
  date: new Date(),
  maxClock: maxClock,
  threads: allThreads.map(thread => ({
    id: thread.internalId,
    snesId: thread.id,
    type: thread.type.name
  })).reduce((a, b) => {a[b.id] = b; return a;}, {}),
  frames: frames.map(frame => ({
    durationBefore: frame.start,
    ramUsage: frame.ramUsage,
    miniRamUsage: frame.miniRamUsage,
    events: frame.events.map(event => event.toJson())
  }))
}

fs.writeFileSync('bsnes_debug.json', JSON.stringify(json));


dumpUsage(Object.assign({}, miniRamUsageByAddress, ramUsageByAddress));
console.log('');
dumpEventUsage('ThreadEvent', threadEvents);