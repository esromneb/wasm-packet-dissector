# Docker
Trying:

* docker run -dit --name emsdk3 -v ~/work:/src emscripten/emsdk:3.1.15 bash
* docker run -dit --name emsdk3 -v ~/work:/src -u $(id -u):$(id -g) emscripten/emsdk:3.1.15 bash
