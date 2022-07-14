const dis = require('../../c/out/dis.js');


let printAThing;
let wiresharkShared;
let registerAll;

// fixme find a nice way to get the binds
function setupBinds() {
  printAThing = dis.cwrap('printAThing', 'void');
  wiresharkShared = dis.cwrap('wrapper_wireshark_shared', 'void', ['number']);
  registerAll = dis.cwrap('registerAll', 'void');
}


beforeAll((done) => {
  dis['onRuntimeInitialized'] = () => {
    console.log('got onRuntimeInitialized');
    
    dis.callMain('');

    setupBinds();

    done();
  }
})

// test('async test', done => {
//   console.log('top of js');
//   done();
// });

// test('sync test', () => {

//   console.log('sync test');

//   printAThing();

//   wiresharkShared(42);

//   console.log('below sync test');
// });

test('sync test', () => {

  console.log('------------------------------');

  registerAll();

  
});



// test('test again', done => {
//   console.log('top of js 2');
//   done();
// });