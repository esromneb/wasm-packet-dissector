# GLIB
GLIB took me awhile to get working. But it turns out that https://github.com/kleisauke/wasm-vips has a compiling version that works perfectly. It's not packaged for export however. I took these steps to get the `c/3rd/zlib-glib/ffi` folder:

# Staring point
git@github.com:kleisauke/wasm-vips.git  4f0184d


# Steps
```plaintext
Deleted 2nd half of build (line 209 and below)
built the docker given
ran it inside the docker


tar with:

  cd /src/wasm-vips/build
  tar -czf /src/emcc-zlib-glib-ffi.tar.gz target/

un tar and then:
  mv target zlib-glib-ffi
```

# References
* see https://github.com/emscripten-core/emscripten/issues/11066