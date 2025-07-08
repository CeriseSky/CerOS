#!/bin/sh

set -e

pushd $(dirname $0)
gcc main.c -o $(basename $PWD).out
popd

