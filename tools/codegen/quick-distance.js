const table = [];
for (var y=0; y<0x40; y++) {
  for (var x=0; x<0x40; x++) {
    var fromX = x << 4;
    var fromY = y << 4;
    table.push(Math.round(Math.sqrt((fromX * fromX) + (fromY * fromY))) >> 3);
  }
}






Object.defineProperty(Array.prototype, 'chunk', {
  value: function(chunkSize) {
  var R = [];
  for (var i = 0; i < this.length; i += chunkSize)
    R.push(this.slice(i, i + chunkSize));
  return R;
}});

console.log('module Aoba::Math');
console.log('  datablock quick_distance_table')
console.log(table.map(item => '$' + item.toString(16).padStart(2, '0')).chunk(16).map(
  row => `    db   ${row.join(', ')}`
).join('\n'));
console.log('  end')
console.log('end')

