# Docker
Trying:

* `docker run -dit --name emsdk3 -v ~/work:/src emscripten/emsdk:3.1.15 bash`
* `docker run -dit --name emsdk3 -v ~/work:/src -u $(id -u):$(id -g) emscripten/emsdk:3.1.15 bash`

* Shows how to run docker with --rm
  * https://hub.docker.com/r/emscripten/emsdk


# Serve
```bash
cd c/out
python3 -m http.server
```



# emcc compile
```bash
cd /src/wasm-packet-dissector/c
NOOPT=1 make
```

# linking against glib
on node 14, I can run this:

```bash
node --experimental-wasm-threads ./node_modules/jest/bin/jest.js --runInBand --no-cache src/test/test_basic.ts
```

# Linking against anything
If there are things I can link outright to, that just work from wireshark I should
I should only hand edit the "important files" and hand build them myself from inside the wasm


# Keywords
WWPD - wireshark wasm packet dissector, used for notes aka fixme


# Notes about race-condition at startup
```
Racecondition between module loading and first test running
"Another option is to use the MODULARIZE option, using -sMODULARIZE. That puts all of the generated JavaScript into a factory function, which you can call to create an instance of your module. The factory function returns a Promise that resolves with the module instance. The promise is resolved once it’s safe to call the compiled code, i.e. after the compiled code has been downloaded and instantiated. For example, if you build with -sMODULARIZE -s 'EXPORT_NAME="createMyModule"', then you can do this:

createMyModule(/* optional default settings */).then(function(Module) {
  // this is reached when everything is ready, and you can call methods on Module
});"
```
* https://emscripten.org/docs/getting_started/FAQ.html





# Sharkd
```json
{"req":"load", "file":"/src/wireshark/py/foo_flags.pcapng"}
{"req":"frame", "frame":4, "proto":"true"}
```
