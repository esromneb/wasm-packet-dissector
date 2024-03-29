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

DEBUG_SOURCE_MAP_FLAG=-g -fdebug-compilation-dir='..'

ifdef NODEBUG
  DEBUG_SOURCE_MAP_FLAG=
endif


# warning and error flags
CLANG_WARN_FLAGS = \
-Wall -Wextra \
-Wno-ignored-qualifiers \
-Wundef \
-Wno-unused-parameter \
-Wno-unused-variable \
-Werror=return-type \
-Wno-c++11-narrowing \
$(DEBUG_SOURCE_MAP_FLAG)

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
-I./3rd/wireshark/tools/lemon \
-I./3rd/wireshark/dfilter \
-I./3rd/wireshark-compat \
-I./3rd/zlib-glib-ffi/include/glib-2.0 \
-I./3rd/zlib-glib-ffi/lib/glib-2.0/include \
-I./3rd/zlib-glib-ffi/include/glib-2.0/glib


# These have a dedicated target below
OBJS = \
build/main.o \
build/wireshark.o \
build/packet-foo.o \
build/sharkd.o \
build/frame_tvbuff.o \
build/cfile.o \
build/sharkd_daemon.o \
build/sharkd_session.o \



# These use a target with a % below
# dfilter-macro.o
# dfilter.o
# stats_tree.o
OBJS3 = \
except.o \
tvbuff_real.o \
tvbuff.o \
tvbparse.o \
time_util.o \
unicode-utils.o \
ws_mempbrk.o \
to_str.o \
oids.o \
osi-utils.o \
addr_and_mask.o \
strutil.o \
address_types.o \
addr_resolv.o \
inet_addr.o \
charsets.o \
register.o \
guid-utils.o \
expert.o \
packet.o \
str_util.o \
dissectors.o \
decode_as.o \
cmdarg_err.o \
value_string.o \
timestamp.o \
column.o \
column-utils.o \
dfunctions.o \
dfvm.o \
drange.o \
gencode.o \
semcheck.o \
sttype-function.o \
sttype-integer.o \
sttype-pointer.o \
sttype-range.o \
sttype-set.o \
sttype-string.o \
sttype-test.o \
syntax-tree.o \
wsjson.o \
json_dumper.o \
nstime.o \
stat_tap_ui.o \
buffer.o \
unit_strings.o \
in_cksum.o \
tfs.o \
tvbuff_subset.o \
color_filters.o \
frame_data_sequence.o \
frame_data.o \
conversation_table.o \
sequence_analysis.o \
export_object.o \
srt_table.o \
rtd_table.o \
follow.o \
tap.o \
strtoi.o \
tap-rtp-common.o \
tap-rtp-analysis.o \
proto_data.o \
rtp_stream_id.o \
io_graph_item.o \
packet-rtp.o \
jsmn.o \
proto.o \
epan.o \


# color_filters.o \

# strptime.o \

OBJS3_A = $(addprefix build/,$(OBJS3))


# EPAN_SRCS = \
# except.c \
# tvbuff_real.c \

# EPAN_SRCS2 = $(addprefix 3rd/wireshark/epan/,$(EPAN_SRCS))
# EPAN_OBJS = $(addprefix build/,$(EPAN_SRCS:.c=.o))

# foo:
# 	@echo ${EPAN_SRCS}
# 	@echo ---
# 	@echo ${EPAN_SRCS2}
# 	@echo ---
# 	@echo ${EPAN_OBJS}
# 	@echo ---
# 	@echo ${OBJS3}
# 	@echo ---
# 	@echo ${OBJS3_A}

# EPAN_OBJS = \
# except.o \
# tvbuff_real.o \




BUILDA = \
3rd/wireshark/epan/ftypes/build/libftypes.a \
3rd/wireshark/wiretap/build/libwiretap.a \
3rd/wireshark/epan/wmem/build/libwmem.a \

# 3rd/wireshark/epan/dfilter/build/libdfilter.a \


# Fix warning with
# https://stackoverflow.com/questions/64588986/how-to-solve-extra-exported-runtime-methods-exception-when-cmake-used-with-emscr

# --shell-file $(TEMPLATE_FILE)
# 	--proxy-to-worker
# 	-s USE_PTHREADS=1 -s PROXY_TO_PTHREAD=1 -s ALLOW_BLOCKING_ON_MAIN_THREAD=0
$(WASM_OUT): $(OBJS) $(OBJS3_A) $(TEMPLATE_FILE) $(JS_TEMPLATE_FILE) $(BUILDA) Makefile
	emcc -s WASM=1 -o $(HTML_OUT) \
	-s ALLOW_MEMORY_GROWTH=1 \
	--pre-js $(JS_TEMPLATE_FILE) \
	-s EXPORTED_FUNCTIONS='[$(EXPORT_STRING) "_main"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "callMain"]' \
	-sLLD_REPORT_UNDEFINED \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS) \
	-L./3rd/zlib-glib-ffi/lib -lglib-2.0 \
	$(BUILDA) \
	$(OBJS) \
	$(OBJS3_A)


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

# build/epan.o: 3rd/wireshark/epan/epan.cpp 3rd/wireshark/epan/epan.h
# 	emcc $< -c -o $@ \
# 	$(INCLUDE) \
# 	'-std=c++2a' \
# 	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)


build/sharkd.o: 3rd/wireshark/sharkd.c 3rd/wireshark/sharkd.h
	emcc $< -c -o $@ \
	$(INCLUDE) \
	'-std=c++2a' \
	-x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/sharkd_daemon.o: 3rd/wireshark/sharkd_daemon.c
	emcc $< -c -o $@ \
	$(INCLUDE) \
	'-std=c++2a' \
	-x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/sharkd_session.o: 3rd/wireshark/sharkd_session.c
	emcc $< -c -o $@ \
	$(INCLUDE) \
	'-std=c++2a' \
	-x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/frame_tvbuff.o: 3rd/wireshark/frame_tvbuff.c 3rd/wireshark/frame_tvbuff.h
	emcc $< -c -o $@ \
	$(INCLUDE) \
	'-std=c++2a' \
	-x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/cfile.o: 3rd/wireshark/cfile.c 3rd/wireshark/cfile.h
	emcc $< -c -o $@ \
	$(INCLUDE) \
	'-std=c++2a' \
	-x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)




# ok so these two rules both match from build
# but they look in different places
# this will work great unless we have multiple files with the same name
# in different dirs
# IF we do do that, we can make 2 build dirs and solve it

build/%.o: 3rd/wireshark/ui/%.c
	@echo "ui match"
	emcc $< -c -o $@ \
	$(INCLUDE) \
	-x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/%.o: 3rd/wireshark/epan/%.c
	@echo "epan match"
	emcc $< -c -o $@ \
	$(INCLUDE) \
	-x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/%.o: 3rd/wireshark/epan/dissectors/%.c
	@echo "epan/dissectors match"
	emcc $< -c -o $@ \
	$(INCLUDE) \
	-x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/%.o: 3rd/wireshark/epan/dfilter/%.c
	@echo "epan/dfilter match"
	emcc $< -c -o $@ \
	$(INCLUDE) \
	-x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/%.o: 3rd/wireshark/wsutil/%.c
	@echo "ws util match"
	emcc $< -c -o $@ \
	$(INCLUDE) \
	-x c++ \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)




# %.o: $$(addprefix 3rd/wireshark/epan/,%.c)
# 	@echo $^

# $(EPAN_OBJS): $(EPAN_SRCS2)
# 	@echo $*
# 	@echo -------
# 	@echo $<
# 	@echo -------
# 	@echo $@
# 	@echo -------
# 	@echo $?
# 	@echo -------
# 	@echo $^
# 	@echo -------
# 	@echo $(<F)


# $(EPAN_OBJS): $(SRCS2)
# 	emcc $(addprefix 3rd/wireshark/epan/,$*).c -c -o $@ \
#  	-x c++ \
# 	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

# %.o : %.c
# 	emcc -c \
# 	$(INCLUDE) \
#  	-x c++ \
# 	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS) \
# 	$< -o $(BUILD_DIR)/$@



# @for objj in $(EPAN_OBJS); do \
# 	$(MAKE) -C $$dir/test all || exit $?; \
# done

# $(addprefix build/,$(EPAN_OBJS)): 3rd/wireshark/epan/%.c
# 	echo $@
# 	echo $(subst .o,.c,$@)
# 	touch $@
# 	emcc $< -c -o $@ \
# 	$(INCLUDE) \
# 	-x c++ \
# 	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)





# build/proto.o: 3rd/wireshark/epan/proto.c 3rd/wireshark/epan/proto.h
# 	emcc -x c++ \
# 	3rd/wireshark/epan/proto.c -c -o build/proto.o \
# 	$(INCLUDE) \
# 	'-std=c++2a' \
# 	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

# build/proto.o: 3rd/wireshark-compat/proto-compat.cpp 3rd/wireshark/epan/proto.h
# 	emcc \
# 	3rd/wireshark-compat/proto-compat.cpp -c -o build/proto.o \
# 	$(INCLUDE) \
# 	'-std=c++2a' \
# 	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)


3rd/wireshark/epan/ftypes/build/libftypes.a: $(wildcard 3rd/wireshark/epan/ftypes/*)
	mkdir -p 3rd/wireshark/epan/ftypes/build && cd 3rd/wireshark/epan/ftypes/build && emcmake cmake .. && make

# 3rd/wireshark/epan/dfilter/build/libdfilter.a: $(wildcard 3rd/wireshark/epan/dfilter/*)
# 	mkdir -p 3rd/wireshark/epan/dfilter/build && cd 3rd/wireshark/epan/dfilter/build && emcmake cmake .. && make

3rd/wireshark/wiretap/build/libwiretap.a: $(wildcard 3rd/wireshark/wiretap/*)
	mkdir -p 3rd/wireshark/wiretap/build/ && cd 3rd/wireshark/wiretap/build/ && emcmake cmake .. && make

3rd/wireshark/epan/wmem/build/libwmem.a: $(wildcard 3rd/wireshark/epan/wmem/*h) $(wildcard 3rd/wireshark/epan/wmem/*c)
	mkdir -p 3rd/wireshark/epan/wmem/build/ && cd 3rd/wireshark/epan/wmem/build/ && emcmake cmake .. && make


clean:
	rm -rf build/*
	rm -rf out/*
	rm -rf 3rd/wireshark/epan/ftypes/build/
	rm -rf 3rd/wireshark/wiretap/build/
	rm -rf 3rd/wireshark/epan/dfilter/build/
	rm -rf 3rd/wireshark/epan/wmem/build/

# 	-sERROR_ON_UNDEFINED_SYMBOLS=0 \