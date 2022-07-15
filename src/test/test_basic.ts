const dis = require('../../c/out/dis.js');

// a Typescript type which must match the javascript version of the
// function signature. It must match what cwrap returns.
interface IAddFromCpp {
  (x: number, y: number): number;
}


let printAThing;
let addFromCpp: IAddFromCpp;

function setupBinds() {
  printAThing = dis.cwrap('printAThing', 'void');

  // the signature of the function here must match what's in the main.cpp
  addFromCpp = dis.cwrap('addFromCpp', 'int', ['int','int']);
}


beforeAll((done) => {
  // a signal from emscripten that our WASM has loaded and is ready to call
  // functions. this is NOT a signal that main() has run.
  // this only works if c/template/pre.js has this line:
  //    Module['noInitialRun'] = true;
  // we use this setup variable set to true, combined with beforeAll()
  // so that tests can run deterministically
  dis['onRuntimeInitialized'] = () => {

    // call main (not required if you want to call main from a specific test)
    dis.callMain('');

    // call our own function which uses cwrap
    setupBinds();

    done();
  }
})

test('async test', done => {
  console.log('top of async test');

  printAThing();

  setTimeout(()=>{
    printAThing();
    done();
  },100);

});

test('sync test', () => {
  console.log('top of sync test');
  
  const a = 4;
  const b = 38;
  const expected = a+b;

  // got doesn't need a type because it will be assigned one from IAddFromCpp
  const got = addFromCpp(a,b);

  expect(got).toEqual(expected);

  // Typescript makes this illegal:
  //   const foo: string = got;

  console.log("Cpp added two numbers: " + got);
});
