.PHONY: build buildw dev start test jestc ctest pretest lint clean

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
