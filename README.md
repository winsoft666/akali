[>>> 中文版](README_ch.md)

# Akali
A common C++ library that don't depends on any library.

---

# Functions
|File|Function|Support Platform|
|---|---|---|
|base64.h <br/> base64.cpp|Base64 encode, decode|Windows <br/> Linux|
|buffer_queue.h <br/> buffer_queue.cpp|Buffer queue that based linked list, it can auto increase size, support fetch data across node|Windows <br/> Linux|
|byteorder.h <br/> byteorder.cpp|Get byte order|Windows <br/> Linux|
|constructormagic.h|Macro: Disable copy-constructor, assign-constructor|Windows <br/> Linux|
|criticalsection.h <br/> criticalsection.cpp|Windows Critical Section helper|Windows|
|deprecation.h|Mark interface deprecation|Windows <br/> Linux|
|directory_monitor.h <br/> directory_monitor.cpp|monitor a directory|Windows|
|display_monitors.h <br/> display_monitors.cpp|Enum display monitors with information|Windows|
|driver_info.h <br/> driver_info.cpp|Get disk driver information|Windows|
|endianess_detect.h <br/> endianess_detect.cpp|Little Endianess/Big Endianess Detection|Windows <br/> Linux|
|file_info.h <br/> file_info.cpp|Get file information(Version, Company name)|Windows|
|ini.h <br/> ini.cpp|INI config file|Windows|
|internet_availability_checker.h <br/> internet_availability_checker.cpp|Check whether internet is avaliable|Windows|
|ipc.h <br/> ipc.cpp|internal process commutication based on named pipe|Windows|
|md5.h <br/> md5.cpp|md5 for string, file|Windows <br/> Linux|
|memory_pool.hpp|C++ memeory pool implement|Windows <br/> Linux|
|noncopyable.h|class uncopyable|Windows <br/> Linux|
|os_ver.h <br/> os_ver.cpp|Get Windows system version|Windows|
|pc_info.h <br/> pc_info.cpp|Get compture infomation(IP, MAC, CPU ID)|Windows|
|process_util.h <br/> process_util.cpp|Query, Close, add UIPI filter|Windows|
|process.hpp <br/> process_win.cpp <br/> process_unix.cpp|Start/Kill process...|Windows <br/> Linux|
|random.h <br/> random.cpp|Generate random number|Windows <br/> Linux|
|registry.h <br/> registry.cpp|Register wirte/read|Windows|
|macros.h|Macros：release/close safely...|Windows <br/> Linux|
|schedule_task.h <br/> schedule_task.cpp|Add, update, remove windows schedule task|Windows|
|scoped_com_initializer.h <br/> scoped_propvariant.h <br/> scoped_variant.h|COM, PROPVARIANT initialize/uninitialize|Windows|
|scoped_variant.h <br/> scoped_variant.cpp|VARIANT helper|Windows|
|shortcut.h <br/> shortcut.cpp|Create, update, delete shortcut|Windows|
|singleton.hpp|C++ class singleton, process singleton|Windows <br/> Linux|
|stack_walker.h <br/> stack_walker.cpp|Get code call stack|Windows|
|string_helper.hpp|std::string helper|Windows <br/> Linux|
|stringencode.h <br/> stringencode.cpp|string encode|Windows <br/> Linux|
|thread.h <br/> thread.cpp|C++ thread helper|Windows <br/> Linux|
|thread_pool.h|C++ thread pool|Windows <br/> Linux|
|timer.h <br/> timer.cpp|timer|Windows <br/> Linux|
|timeutils.h <br/> timeutils.cpp|time helper|Windows <br/> Linux|
|win_main.h <br/> win_main.cpp|An easy way to generate dump|Windows|
|win_service_base.h <br/> win_service_base.cpp <br/> win_service_installer.h <br/> win_service_installer.cpp|Install, Uninstall, Start, Stop, Puased windows service|Windows|
|ping.h <br/> ping.cpp|Ping implement|Windows|
|host_resolve.h <br/> host_resolve.cpp|Host resolve|Windows|
|networkprotocoldef.h|IP, ICPM protocol header|Windows <br/> Linux|
|ipaddress.h <br/> ipaddress.cpp|IP address|Windows <br/> Linux|
|socketaddress.h <br/> socketaddress.cpp|Socket address|Windows <br/> Linux|
|iocp_socket.h <br/> iocp_socket.cpp <br/> iocpserver.h <br/> iocpserver.cpp|Windows IOCP network model, IOCP server|Windows|
|overlappedsocket.h <br/> overlappedsocket.cpp|Windows Overlapped Socket|Windows|
|miscellaneous.h <br/> miscellaneous.cpp|miscellaneous functions, such as screenshot, version string compare, console colorful progress bar|Windows <br/> Linux|
|filesystem.hpp|An implementation of C++17 std::filesystem|Windows <br/> Linux|

---

# Compile
Using `CMake` to generate visual studio project or makefile, then compile.

CMake paramenters:

- BUILD_SHARED_LIBS: whether dynamic library or not, `ON` or `OFF`.
- BUILD_TESTS: whether compile test project or not, `ON` or `OFF`.

```bash
# Windows Sample
cmake.exe -G "Visual Studio 15 2017" -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF -S %~dp0 -B %~dp0build

# Linux
cmake -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF
make
```

---

# Install with vcpkg
```bash
vcpkg install akali
```
