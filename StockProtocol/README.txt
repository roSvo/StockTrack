Building for windows.

Ensure you have projcet hierarchy as :

StockTrack
-StockProtocol
--build
---Desktop_Qt_6_7_2_MinGW_64_bit-Debug
----cmake_install.cmake
--header
--src
--CMakeLists.linux.txt
--CMakeLists.txt
--README.txt

-StockTrackClient
--build
--header
--lib
--qml
--src

Inside Desktop_Qt_6_7_2_MinGW_64_bit-Debug folder, where the cmake_install.cmake file is located call:
"C:/Qt/Tools/CMake_64/bin/cmake.exe" ..

If you don't have CMake in path variable use Qt's standard cmake installation (by default located at :
"C:/Qt/Tools/CMake_64/bin/cmake.exe" --install .

When moving to Linux, rename CMakeLitst.txt -> CMakeLists.windows.txt and rename CMakeLists.linux.txt to CMakeLists.txt. 

Create build directory with mkdir build
Change into it 'cd build'
cmake ..
cmake --build . #or make
cmake --install .
Done.
