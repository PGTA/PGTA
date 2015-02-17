#!/bin/bash

./../external/build-tools/bin/premake/premake5_osx clean --file=PGTA.lua
./../external/build-tools/bin/premake/premake5_osx gmake --file=PGTA.lua

pushd gmake/ 
make clean
make -j16
popd
