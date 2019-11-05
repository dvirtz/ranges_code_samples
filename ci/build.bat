conan remote add onebaseroach https://api.bintray.com/conan/y4n9squared/onebaseroach
conan remote add mputsz https://api.bintray.com/conan/mpusz/conan-mpusz
cmake -S %SOURCE_FOLDER% -B build -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DRUN_TESTS_POSTBUILD=ON
cmake --build ./build