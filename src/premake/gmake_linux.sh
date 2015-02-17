#!/bin/bash

./../external/build-tools/bin/premake/premake5_linux clean --file=PGTA.lua
# Check the exit status
if [ "$?" != "0" ]; then
	echo "premake5_linux clean --file=PGTA.lua failed"
	exit 1
fi
./../external/build-tools/bin/premake/premake5_linux gmake --file=PGTA.lua
if [ "$?" != "0" ]; then
	echo "premake5_linux gmake --file=PGTA.lua failed"
	exit 1
fi
pushd gmake/ 
make clean
if [ "$?" != "0" ]; then
	echo "make clean failed"
	exit 1
fi
make -j16
if [ "$?" != "0" ]; then
	echo "make failed"
	exit 1
fi
popd
