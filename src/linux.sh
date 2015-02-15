#!/bin/bash

./external/build-tools/bin/premake/premake5_linux clean --file=premake4.lua
./external/build-tools/bin/premake/premake5_linux gmake --file=premake4.lua

pushd build/ 
make clean
make
popd
