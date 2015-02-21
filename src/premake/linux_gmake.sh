#!/bin/bash

runCommand()
{
    echo $1 && $1
    if [ "$?" != "0" ]; then
        echo "ERROR : $1" && exit 1
    fi
}

runCommand './../external/build-tools/bin/premake/premake5_linux clean --file=PGTA.lua'
runCommand './../external/build-tools/bin/premake/premake5_linux gmake --file=PGTA.lua'

pushd gmake/

runCommand 'make clean'
runCommand 'make'

popd
