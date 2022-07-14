// import("c/out/dis.js")

const dis = require('../../c/out/dis.js');
// const hellowasm = require('../src/rs/hello-wasm/pkg/hello_wasm.js');


test('bz 2', done => {

  console.log('async test');

  dis['onRuntimeInitialized'] = () => {
    console.log('got onRuntimeInitialized');
    
    dis.callMain('');
    
    done();
  }



//Module["onRuntimeInitialized"]

  // setTimeout(()=>{
  // },1000)

});
