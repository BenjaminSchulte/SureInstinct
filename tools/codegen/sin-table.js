const table = [];
for (let angle=0; angle<0x140; angle++) {
  table.push(Math.round(Math.sin((angle + 0x100) / 0x100 * Math.PI * 2) * 0x100));
}





Object.defineProperty(Array.prototype, 'chunk', {
  value: function(chunkSize) {
  var R = [];
  for (var i = 0; i < this.length; i += chunkSize)
    R.push(this.slice(i, i + chunkSize));
  return R;
}});

function signed(num) {
  if (num < 0) {
    return 0x10000 + num;
  } else {
    return num;
  }
}

console.log('module Aoba::Math');
console.log('  @locate_at align: $100')
console.log('  datablock sin_table')
console.log(table.map(item => '$' + signed(item).toString(16).padStart(4, '0')).chunk(8).map(
  row => `    dw   ${row.join(', ')}`
).join('\n'));
console.log('  end')
console.log('  cos_table = sin_table.to_sym + $40 * 2')
console.log('end')

