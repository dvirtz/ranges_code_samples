#!/bin/sh

conan remote add onebaseroach https://api.bintray.com/conan/y4n9squared/onebaseroach
conan remote add mputsz https://api.bintray.com/conan/mpusz/conan-mpusz
cmake -S repo -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DRUN_TESTS_POSTBUILD=OFF
cmake --build ./build