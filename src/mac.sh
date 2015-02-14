#!/bin/bash

./external/build-tools/bin/premake/premake5_osx clean --file=pgta.lua
./external/build-tools/bin/premake/premake5_osx gmake --file=pgta.lua

pushd build/ 
make clean
make
popd
