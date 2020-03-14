# Hacky build script for now so I don't have to decide whether to learn cmake, make or bazel.
# If you'd like to replace this with something better, please feel free to!
emcc \
  src/cc/viz.cc \
  src/cc/renderer.cc \
  contrib/jsoncpp/src/lib_json/json_{writer,value}.cpp \
  contrib/abseil-cpp/absl/strings/numbers.cc \
  contrib/abseil-cpp/absl/strings/str_cat.cc \
  -Icontrib/googletest/googletest/include \
  -Icontrib/eigen \
  -Icontrib/abseil-cpp \
  -Icontrib/jsoncpp/include \
  -std=c++17  \
  -s WASM=1 \
  -s USE_WEBGL2=1 \
  -s MIN_WEBGL_VERSION=2 \
  -s MAX_WEBGL_VERSION=2 \
  -O2 \
  -o gen/index.js
