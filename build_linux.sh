#!/bin/bash

mkdir -p bin
cd bin

if [[ $1 = "release" ]]; then
	echo "Building release mode"
	mkdir -p release
	cd release
	cmake -DCMAKE_BUILD_TYPE=Release ../..
elif [[ $1 = "debug" ]]; then
	echo "Building debug mode"
	mkdir -p debug
	cd debug
	cmake -DCMAKE_BUILD_TYPE=Debug ../..
else
	echo "Missing argument 1"
	echo "Usage: sh build_linux.sh [debug/release]"
	exit 1
fi

make

