#!/bin/bash

./premake5_osx clean --file=src/premake4.lua
./premake5_osx gmake --file=src/premake4.lua

pushd src/build/ 
make clean
make
popd
