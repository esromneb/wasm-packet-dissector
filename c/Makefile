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
"_printAThing",


# warning and error flags
CLANG_WARN_FLAGS = \
-Wall -Wextra \
-Wno-ignored-qualifiers \
-Wundef \
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

# --shell-file $(TEMPLATE_FILE)
# 	--proxy-to-worker
$(WASM_OUT): $(WASM_MAIN) $(CPP_FILES) $(HPP_FILES) $(WASM_CPP_FILES) $(TEMPLATE_FILE) $(JS_TEMPLATE_FILE) Makefile
	emcc $(WASM_MAIN) $(WASM_CPP_FILES) $(CPP_FILES) -s WASM=1 -o $(HTML_OUT) \
	-s ALLOW_MEMORY_GROWTH=1 \
	--pre-js $(JS_TEMPLATE_FILE) \
	-s EXPORTED_FUNCTIONS='[$(EXPORT_STRING) "_main"]' \
	-s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
	'-std=c++2a' $(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)


clean:
	rm -rf out/*