#!/bin/bash

./premake5_osx clean --file=src/pgta.lua
./premake5_osx gmake --file=src/pgta.lua

pushd src/build/ 
make clean
make
popd
