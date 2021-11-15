#!/bin/bash

mkdir -p bin
cd bin

if [ $1 = "release" ]; then
	echo "Building release mode"
	mkdir -p release
	cd release
	cmake -DCMAKE_BUILD_TYPE=Release ../..
else
	echo "Building debug mode"
	mkdir -p debug
	cd debug
	cmake -DCMAKE_BUILD_TYPE=Debug ../..
fi

make

