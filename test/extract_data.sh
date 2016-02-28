#!/usr/bin/env bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
INIT_SIZE=500
WORKLOAD_SIZE=500
pushd ${DIR}
(
head -n ${INIT_SIZE} init-file.txt;
echo "S";
head -n ${WORKLOAD_SIZE} workload-file.txt;
echo "F";
) > test-data.txt
(
echo "R";
cat result-file.txt;
) > test-output.txt
popd
