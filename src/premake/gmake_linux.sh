#!/bin/bash

./../external/build-tools/bin/premake/premake5_linux clean --file=PGTA.lua
./../external/build-tools/bin/premake/premake5_linux gmake --file=PGTA.lua

pushd gmake/ 
make clean
make
popd
