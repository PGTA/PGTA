#!/bin/bash

./external/build-tools/bin/premake/premake5_linux clean --file=pgta.lua
./external/build-tools/bin/premake/premake5_linux gmake --file=pgta.lua

pushd build/ 
make clean
make
popd
