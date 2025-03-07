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


Open with Qt to build. From File menu -> Build -> Rebuild
-> this will create build directory and within it you should find Release (or debug, but there is no reson to build anyting but release from this)

Next: 

If you don't have CMake in path variable use Qt's standard cmake installation (by default located at :
"C:/Qt/Tools/CMake_64/bin/cmake.exe" --install .

When moving to Linux, rename CMakeLitst.txt -> CMakeLists.windows.txt and rename CMakeLists.linux.txt to CMakeLists.txt. 

Create build directory with mkdir build
Change into it 'cd build'
cmake ..
cmake --build . #or make
cmake --install .
Done.
