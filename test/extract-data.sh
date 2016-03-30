#!/usr/bin/env bash

(
head -n 1000000 init-file.txt
echo "S"
head -n 1000 workload-file.txt
) > test-data.txt