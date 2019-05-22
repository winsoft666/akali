@echo off
cmake.exe -G "Visual Studio 14" -DCMAKE_TOOLCHAIN_FILE=D:\sourcecode\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows-static -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=D:\ppxbase -DBUILD_TESTS=ON -S %~dp0 -B %~dp0build

pause