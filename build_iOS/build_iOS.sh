cmake .. -DCMAKE_TOOLCHAIN_FILE=ios.toolchain.cmake -DIOS_PLATFORM=SIMULATOR64
make
rm Makefile
rm -r CMakeFiles
rm CMakeCache.txt
rm cmake_install.cmake
mv libjsmn.a libjsmn_x86_64.a
echo ">>>>>>>>>>>>>>>>>>>>>>>>>x86_64 finished"
cmake .. -DCMAKE_TOOLCHAIN_FILE=ios.toolchain.cmake -DIOS_PLATFORM=SIMULATOR
make
rm Makefile
rm -r CMakeFiles
rm CMakeCache.txt
rm cmake_install.cmake
mv libjsmn.a libjsmn_i386.a
echo ">>>>>>>>>>>>>>>>>>>>>>>>>i386 finished"
cmake .. -DCMAKE_TOOLCHAIN_FILE=ios.toolchain.cmake -DIOS_PLATFORM=OS
make
rm Makefile
rm -r CMakeFiles
rm CMakeCache.txt
rm cmake_install.cmake
mv libjsmn.a libjsmn_iphoneos.a
echo ">>>>>>>>>>>>>>>>>>>>>>>>>iPhoneOS finished"

lipo -create libjsmn_x86_64.a libjsmn_i386.a libjsmn_iphoneos.a -o libjsmn.a

rm libjsmn_x86_64.a libjsmn_i386.a libjsmn_iphoneos.a

lipo -info libjsmn.a

cp ../jsmn.h jsmn.h