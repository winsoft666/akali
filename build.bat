@echo off
vcpkg install curl[openssl]:x86-windows
vcpkg install cryptopp:x86-windows
vcpkg install gtest:x86-windows

cmake.exe -G "Visual Studio 14" -DCMAKE_TOOLCHAIN_FILE=D:\sourcecode\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=D:\ppx -DBUILD_TESTS=ON -S %~dp0 -B %~dp0build

cmake.exe --build %~dp0build

pause