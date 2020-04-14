#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo $DIR
cd $DIR
mkdir -p cmake-build
cd cmake-build
cmake ../
make
if [ $# -eq 0 ]
then
    ctest
else
    ctest -V
fi
