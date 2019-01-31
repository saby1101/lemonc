mkdir build
cd build
REM ensure you have mingw-w64 (x86-64 version) installed
cmake .. -DCMAKE_BUILD_TYPE=Release  -G "MinGW Makefiles"
mingw32-make
cd ..
