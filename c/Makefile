# A .html .js and .wasm are all generated from
# this name
BASE_NAME=dis

HTML_OUT=out/$(BASE_NAME).html
WASM_OUT=out/$(BASE_NAME).wasm

JS_TEMPLATE_FILE = template/pre.js


all: wasm

wasm: $(WASM_OUT)


WASM_MAIN=src/main.cpp

HPP_FILES =

CPP_FILES =

# WASM only cpp files
WASM_CPP_FILES =


# this is a list of all C functions we want to publish to javascript
# In the main cpp file, each of these is wrapped in extern "C" {}
# the version here has a prepended underscore
# ALL lines (including final) must have trailing comma
EXPORT_STRING = \
"_printAThing", \
"_addFromCpp", \
"_wrapper_core_shared",


# warning and error flags
CLANG_WARN_FLAGS = \
-Wall -Wextra \
-Wno-ignored-qualifiers \
-Wundef \
-Wno-unused-parameter \
-Wno-unused-variable \
-Werror=return-type
# -Wconversion
# -Wshadow

CLANG_O_FLAG = '-O3'

ifdef NOOPT
  CLANG_O_FLAG =
endif

ifdef OPT3
  CLANG_O_FLAG = '-O3'
endif

INCLUDE = \
-I./src \
# -I./3rd/whatever \


# Fix warning with
# https://stackoverflow.com/questions/64588986/how-to-solve-extra-exported-runtime-methods-exception-when-cmake-used-with-emscr

# --shell-file $(TEMPLATE_FILE)
# 	--proxy-to-worker
# 	-s USE_PTHREADS=1 -s PROXY_TO_PTHREAD=1 -s ALLOW_BLOCKING_ON_MAIN_THREAD=0
$(WASM_OUT): build/main.o build/program_core.o $(TEMPLATE_FILE) $(JS_TEMPLATE_FILE) Makefile
	emcc -s WASM=1 -o $(HTML_OUT) \
	-s ALLOW_MEMORY_GROWTH=1 \
	--pre-js $(JS_TEMPLATE_FILE) \
	-s EXPORTED_FUNCTIONS='[$(EXPORT_STRING) "_main"]' \
	-s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "callMain"]' \
	-sLLD_REPORT_UNDEFINED \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS) \
	build/main.o build/program_core.o

build/main.o: src/main.cpp
	emcc src/main.cpp -c -o build/main.o \
	$(INCLUDE) \
	'-std=c++2a' \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/program_core.o: src/program_core.cpp src/program_core.hpp
	emcc src/program_core.cpp -c -o build/program_core.o \
	$(INCLUDE) \
	'-std=c++2a' \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)


# build/proto.o: 3rd/wireshark/epan/proto.c 3rd/wireshark/epan/proto.h
# 	emcc -x c++ \
# 	3rd/wireshark/epan/proto.c -c -o build/proto.o \
# 	$(INCLUDE) \
# 	'-std=c++2a' \
# 	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

clean:
	rm -rf build/*
	rm -rf out/*