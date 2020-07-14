#!/bin/sh

export CC="i686-pc-serenity-gcc"
export CXX="i686-pc-serenity-g++"
export PATH="${SERENITY_ROOT}/Toolchain/Local/bin:${PATH}"

commit="6075defd55e66a3c627cb6912a1431971eaa1ed6"
js_cpp_url="https://raw.githubusercontent.com/SerenityOS/serenity/${commit}/Userland/js.cpp"

rm -f src/jsb.cpp
curl $js_cpp_url > src/jsb.cpp
patch -p1 src/jsb.cpp patches/jsb.patch
mkdir -p Build
cd Build
cmake .. -DCMAKE_TOOLCHAIN_FILE="${SERENITY_ROOT}/Toolchain/CMakeToolchain.txt"
make
make install
