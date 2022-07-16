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
"_registerName", \
"_registerAll", \
"_printAThing", \
"_doCall", \
"_wrapper_wireshark_shared",


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
-I./3rd/wireshark \
-I./3rd/wireshark/epan \
-I./3rd/wireshark-compat \
-I./3rd/zlib-glib-ffi/include/glib-2.0 \
-I./3rd/zlib-glib-ffi/lib/glib-2.0/include \
-I./3rd/zlib-glib-ffi/include/glib-2.0/glib

OBJS = \
build/main.o \
build/wireshark.o \
build/packet-foo.o \
build/proto.o \
build/strutil.o \
build/to_str.o \
build/epan.o

BUILDA = \
3rd/wireshark/epan/ftypes/build/libftypes.a


# Fix warning with
# https://stackoverflow.com/questions/64588986/how-to-solve-extra-exported-runtime-methods-exception-when-cmake-used-with-emscr

# --shell-file $(TEMPLATE_FILE)
# 	--proxy-to-worker
# 	-s USE_PTHREADS=1 -s PROXY_TO_PTHREAD=1 -s ALLOW_BLOCKING_ON_MAIN_THREAD=0
$(WASM_OUT): $(OBJS) $(TEMPLATE_FILE) $(JS_TEMPLATE_FILE) $(BUILDA) Makefile
	emcc -s WASM=1 -o $(HTML_OUT) \
	-s ALLOW_MEMORY_GROWTH=1 \
	--pre-js $(JS_TEMPLATE_FILE) \
	-s EXPORTED_FUNCTIONS='[$(EXPORT_STRING) "_main"]' \
	-s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "callMain"]' \
	-sLLD_REPORT_UNDEFINED \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS) \
	-L./3rd/zlib-glib-ffi/lib -lglib-2.0 \
	-L./3rd/ -lwmem \
	$(BUILDA) \
	$(OBJS)


build/main.o: src/main.cpp
	emcc src/main.cpp -c -o build/main.o \
	$(INCLUDE) \
	'-std=c++2a' \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/wireshark.o: 3rd/wireshark/wireshark.cpp 3rd/wireshark/wireshark.h
	emcc 3rd/wireshark/wireshark.cpp -c -o build/wireshark.o \
	$(INCLUDE) \
	'-std=c++2a' \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/packet-foo.o: src/plugins/foo/packet-foo.cpp
	emcc src/plugins/foo/packet-foo.cpp -c -o build/packet-foo.o \
	$(INCLUDE) \
	'-std=c++2a' \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/epan.o: 3rd/wireshark/epan/epan.cpp 3rd/wireshark/epan/epan.h
	emcc $< -c -o $@ \
	$(INCLUDE) \
	'-std=c++2a' \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/strutil.o: 3rd/wireshark/epan/strutil.c 3rd/wireshark/epan/strutil.h
	emcc $< -c -o $@ \
	$(INCLUDE) \
 	'-std=c++2a' -x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/to_str.o: 3rd/wireshark/epan/to_str.c 3rd/wireshark/epan/to_str.h
	emcc $< -c -o $@ \
	$(INCLUDE) \
 	-x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

# build/ftypes.o: 3rd/wireshark/epan/ftypes/ftypes.c 3rd/wireshark/epan/ftypes/ftypes.h
# 	emcc $< -c -o $@ \
# 	$(INCLUDE) \
# 	'-std=c++2a' -x c++ \
# 	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

# build/proto.o: 3rd/wireshark/epan/proto.c 3rd/wireshark/epan/proto.h
# 	emcc -x c++ \
# 	3rd/wireshark/epan/proto.c -c -o build/proto.o \
# 	$(INCLUDE) \
# 	'-std=c++2a' \
# 	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/proto.o: 3rd/wireshark-compat/proto-compat.cpp 3rd/wireshark/epan/proto.h
	emcc \
	3rd/wireshark-compat/proto-compat.cpp -c -o build/proto.o \
	$(INCLUDE) \
	'-std=c++2a' \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)


3rd/wireshark/epan/ftypes/build/libftypes.a:
	mkdir -p 3rd/wireshark/epan/ftypes/build && cd 3rd/wireshark/epan/ftypes/build && emcmake cmake .. && make

clean:
	rm -rf build/*
	rm -rf out/*
	rm -rf 3rd/wireshark/epan/ftypes/build

# 	-sERROR_ON_UNDEFINED_SYMBOLS=0 \