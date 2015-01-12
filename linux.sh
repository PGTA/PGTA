#!/bin/bash

./premake5_linux clean --file=src/pgta.lua
./premake5_linux gmake --file=src/pgta.lua

pushd src/build/ 
make clean
make
popd
