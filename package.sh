#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
/bin/tar czf submission.tar.gz run.sh compile.sh package.sh README Makefile src

