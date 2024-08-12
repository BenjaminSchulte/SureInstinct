function snesTan(angle) { return Math.min(0xFFFF, Math.round(Math.tan(angle/0x40 * Math.PI * 0.5) * 0x100)); }

const table = [];
let previous = 0;
for (let angle=0; angle<=0x40; angle++) {
  let current = snesTan(angle);
  if (angle > 0) table.push(((current - previous) >> 1) + previous);
  previous = current;
}
table.push(0xFFFF);





Object.defineProperty(Array.prototype, 'chunk', {
  value: function(chunkSize) {
  var R = [];
  for (var i = 0; i < this.length; i += chunkSize)
    R.push(this.slice(i, i + chunkSize));
  return R;
}});

console.log('module Aoba::Math');
console.log('  @locate_at align: $100')
console.log('  datablock tan_table')
console.log(table.map(item => '$' + item.toString(16).padStart(4, '0')).chunk(8).map(
  row => `    dw   ${row.join(', ')}`
).join('\n'));
console.log('  end')
console.log('end')

