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