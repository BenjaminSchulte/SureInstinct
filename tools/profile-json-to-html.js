const fs = require('fs');
const json = JSON.parse(fs.readFileSync('./bsnes_debug.json', 'utf-8'));

const ORDERED = true;
const FRAME_HEIGHT = 20;

const html = [];
html.push('<DOCTYPE html>');
html.push('<html><head>');
html.push('<title>Profile</title>');
html.push('<style type="text/css">' + `
body {
  background-color: #444;
  font-family: sans-serif;
}
* {
  box-sizing: border-box;
}
.frames {
  display: block;
  margin: 0;
  padding: 0;
  list-style: none;
}
.frames li {
  display: flex;
  margin: 0;
  padding: 0;
  height: ${FRAME_HEIGHT}px;
}
.frame-number {
  display: block;
  width: 50px;
  text-align: right;
  font-size: 12px;
  color: #aaa;
  padding-right: 5px;
  border-right: 1px solid #888;
}
.frame-miniram {
  width: 30px;
  padding: 0 5px;
  font-size: 8px;
  color: #888;
}
.frame-miniram-low {
  color: #888;
}
.frame-miniram-medium {
  color: #F80;
}
.frame-miniram-high {
  color: red;
}
.frame-resource {
  width: 50px;
  font-weight: bold;
  padding: 0 5px;
}
.frame-summary {
  display: block;
  position: relative;
  flex: 1;
  background: #333;
  box-shadow: inset 2px 2px 2px #2a2a2a;
}
.frame-ram {
  display: block;
  background: #222;
  height: 100%;
}
.frame-ram-usage {
  display: block;
  height: 100%;
}
.frame-ram-usage-low {
  background: lime;
}
.frame-ram-usage-medium {
  background: yellow;
}
.frame-ram-usage-high {
  background: red;
}
.frame-max {
  display: block;
  position: absolute;
  top: 0;
  height: 100%;
  border-left: 1px solid #faa;
  width: 1px;
}
.event {
  display: block;
  position: absolute;
  top: 0;
  font-size: 9px;
  height: 100%;
  overflow: hidden;
  background-color: #555;
  line-height: 9px;
  box-shadow: inset -1px -1px 0 rgba(0,0,0,0.3);
  cursor: default;
}
.irq {
  background-color: #888;
}
.thread-audio {
  background-color: #080;
}
.thread-level {
  background-color: #0f8;
}
.thread-tileset {
  background-color: #08f;
}
.thread-palette {
  background-color: #80f;
}
.thread-script {
  background-color: #f4c;
}
.thread-font {
  background-color: #4cf;
}
.thread-sprite {
  background-color: #f80;
}
.thread-sprite-event-FF { background-color: red; }
.thread-sprite-event-FE { background-color: blue; }
.thread-sprite-event-FD { background-color: lime; }
.thread-sprite-event-FC { background-color: yellow; }
.thread-sprite-event-FB { background-color: white; }
.thread-sprite-event-F0 { background-color: cyan; }
` + '</style>');
html.push('</head><body>');

const VBLANK_TICKS = 56000; //estimated
const TICKS_PER_FRAME = 357400;
const tickMultiply = 100 / json.maxClock;
const frameVBlank = Math.round(VBLANK_TICKS * tickMultiply);
const frameMax = Math.round(TICKS_PER_FRAME * tickMultiply);

html.push('<ul class="frames">')

const eventNames = {
  sprite: {
    'FF': 'NPC col',
    'FE': 'Tile col',
    'FD': 'Tile event',
    'FC': 'Move init',
    'FB': 'Move upd',
    'F0': 'OAM'
  }
}

function toEventName(threadId, eventId) {
  if (eventId === undefined || !eventId) {
    return '';
  }
  const thread = threadId ? json.threads[threadId].type : 'unknown';
  const event = eventId.toString(16).toUpperCase().padStart(2, '0');

  if (eventNames[thread] && eventNames[thread][event]) {
    return eventNames[thread][event];
  }

  return `EVENT ${eventId}`;
}

function eventHash(event) {
  return `${event.thread || 'x'}#${event.event || 'x'}`;
}

function orderIfActive(events) {
  if (!ORDERED || !events.length) {
    return events;
  }

  let clock = events[0].clock;

  const listByDuration = {};
  const eventsByHash = {}
  let index = 0;
  const eventStack = [];
  for (const event of events) {
    if (event.type === 'idle') {
      continue;
    }

    let hash = eventHash(event);
    const nextEvent = events[++index] || null;
    const duration = nextEvent ? (nextEvent.clock - event.clock) : (json.maxClock - event.clock);

    if (event.type === 'event-start' || event.type === 'thread') {
      eventStack.push(hash);
      eventsByHash[hash] = event;
    } else if (event.type === 'event-stop') {
      hash = eventStack.pop();
    }

    listByDuration[hash] ||= 0
    listByDuration[hash] += duration
  }

  let result = Object.keys(listByDuration).sort().map((hash) => {
    const item = {
      type: 'event-start',
      clock: clock,
      thread: eventsByHash[hash].thread,
      event: eventsByHash[hash].event
    }

    clock += listByDuration[hash];

    return item;
  })
  result = result.concat({
    type: 'idle',
    clock: clock,
    thread: 0
  })
  
  return result;
}

const ramLevels = ['low', 'low', 'medium', 'high'];
let number = 1;
for (const frame of json.frames) {
  html.push('<li class="frame">')
  const ramLevel = ramLevels[Math.floor(frame.ramUsage/255/2.55/100*4)];
  const miniRamLevel = frame.miniRamUsage === 0 ? 'low' : (frame.miniRamUsage > 0x200 ? 'high' : 'medium');

  html.push(`<span class="frame-number">${number++}</span>`)
  html.push(`<span class="frame-miniram frame-miniram-${miniRamLevel}">${frame.miniRamUsage.toString(16)}</span>`);
  html.push(`<span class="frame-resource">`);
  html.push(`<span class="frame-ram"><span class="frame-ram-usage frame-ram-usage-${ramLevel}" style="width:${Math.round(frame.ramUsage/255/2.55)}%"></span></span>`);
  html.push(`</span>`);
  html.push(`<span class="frame-summary">`)

  if (frame.events.length) {
    const duration = frame.events[0].clock;
    const width = duration * tickMultiply;
    html.push(`<span class="event irq" style="left:0%;width:${width}%"></span>`)

  }

  let index = 0;
  const events = orderIfActive(frame.events);
  for (const event of events) {
    if (event.type === 'idle') {
      continue;
    }

    const nextEvent = events[++index] || null;
    const duration = nextEvent ? (nextEvent.clock - event.clock) : (json.maxClock - event.clock);
    const left = event.clock * tickMultiply;
    const width = duration * tickMultiply;
    let thread = event.thread ? json.threads[event.thread].type : 'unknown';
    let text = '';
    let br = '';
    if (event.type === 'event-start') {
      br = '<br/>';
      text = `${toEventName(event.thread, event.event)}`
    }

    html.push(`<span class="event thread-${thread} thread-${thread}-event-${(event.event || 0).toString(16).toUpperCase()}" style="left:${left}%;width:${width}%">${thread}${br}${text}</span>`)
  }

  html.push(`<span class="frame-max" style="left:${frameVBlank}%"></span>`)
  html.push(`<span class="frame-max" style="left:${frameMax}%"></span>`)
  html.push(`</span>`)

  html.push('</li>')
}

html.push('</ul>');

html.push('</body>');
html.push('</html>');

fs.writeFileSync('bsnes_debug.html', html.join('\n'));