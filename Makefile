.PHONY: build buildw dev start test jestc ctest pretest lint clean cleanall

build:
	npm run build

buildw:
	npm run buildw

dev:
	npm run dev

start:
	npm run start

test: jestc
	npm run test

jestc:
	npm run jestc

# jest with cache
ctest:
	npm run test

pretest:
	npm run pretest

lint:
	npm run lint

clean:
	npm run clean

cleanall: clean clean-wasm


EMSDK_DOCKER=emscripten/emsdk:3.1.15
DOCKER_RUN=docker run --rm  -v $(PWD):/src -u $(id -u):$(id -g) $(EMSDK_DOCKER)

.PHONY: wasm noopt-wasm clean-wasm wasm-noopt
wasm:
	$(DOCKER_RUN) make -C c wasm

wasm-noopt: noopt-wasm
noopt-wasm:
	$(DOCKER_RUN) /bin/bash -c 'NOOPT=1 make -C c wasm'


# no need to fire up docker, clean can work on host here
clean-wasm:
	make -C c clean
