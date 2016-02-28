#!/usr/bin/env bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
pushd ${DIR}
make
echo "Starting program..."
./sigmod < test/test-data.txt
diff result.txt test/test-output.txt
popd
