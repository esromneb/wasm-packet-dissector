$(WASM_OUT): build/main.o build/wireshark.o $(TEMPLATE_FILE) $(JS_TEMPLATE_FILE) Makefile
	emcc -s WASM=1 -o $(HTML_OUT) \
	-s ALLOW_MEMORY_GROWTH=1 \
	--pre-js $(JS_TEMPLATE_FILE) \
	-s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
	-sLLD_REPORT_UNDEFINED \
	build/main.o build/wireshark.a

build/main.o:
	emcc src/main.cpp -c -o build/main.o \
	$(INCLUDE) \
	'-std=c++2a' \
	-s EXPORTED_FUNCTIONS='[$(EXPORT_STRING) "_main"]' \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/wireshark.o:
	emcc 3rd/wireshark/wireshark.cpp -c -o build/wireshark.o \
	$(INCLUDE) \
	$(CLANG_O_FLAG) $(CLANG_WARN_FLAGS)

build/wireshark.a: build/wireshark.o
	emar cr build/wireshark.a build/wireshark.o 