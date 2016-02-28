#!/usr/bin/env bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
pushd ${DIR}
make
pushd test
make
./harness init-file.txt workload-file.txt result-file.txt ../sigmod
popd
popd
