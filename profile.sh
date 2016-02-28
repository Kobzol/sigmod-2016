#!/usr/bin/env bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
pushd ${DIR}
make debug
valgrind --tool=callgrind --separate-threads=yes ./sigmod < test/test-data.txt > result.txt
popd
