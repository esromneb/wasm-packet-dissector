# Docker
Trying:

* `docker run -dit --name emsdk3 -v ~/work:/src emscripten/emsdk:3.1.15 bash`
* `docker run -dit --name emsdk3 -v ~/work:/src -u $(id -u):$(id -g) emscripten/emsdk:3.1.15 bash`




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
