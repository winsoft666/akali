[中文版](README_ch.md)
# 1. Project Introduction
[ppxbase](https://github.com/winsoft666/ppxbase) is a common C++ library, don't depend on other library.

# 2. Function Module
|File|Function|Support Platform|
|---|---|---|
|assert.h, assert.cpp|Runtime assert, support addtional message|Windows, Linux|
|base64.h, base64.cpp|Base64 encode, decode|Windows, Linux|
|buffer_queue.h, buffer_queue.cpp|Buffer queue that based linked list, it can auto increase size, support fetch data across node|Windows, Linux|
|byteorder.h, byteorder.cpp|Get byte order|Windows, Linux|
|cmdline_parse.h, cmdline_parse.cpp|command line parse|Windows, Linux|
|flags.h, flags.cpp|Define command line parameter and parse it|Windows, Linux|
|constructormagic.h|Macro: Disable copy-constructor, assign-constructor|Windows, Linux|
|criticalsection.h, criticalsection.cpp|Windows Critical Section helper|Windows|
|deprecation.h|Mark interface deprecation|Windows, Linux|
|directory_monitor.h, directory_monitor.cpp|monitor a directory|Windows|
|display_monitors.h, display_monitors.cpp|Enum display monitors with information|Windows|
|driver_info.h, driver_info.cpp|Get disk driver information|Windows|
|endianess_detect.h, endianess_detect.cpp|Little Endianess/Big Endianess Detection|Windows, Linux|
|file_info.h, file_info.cpp|Get file information(Version, Company name)|Windows|
|ini.h, ini.cpp|INI config file|Windows|
|internet_availability_checker.h, internet_availability_checker.cpp|Check whether internet is avaliable|Windows|
|ipc.h, ipc.cpp|internal process commutication based on named pipe|Windows|
|logging.h, logging.cpp|logging, output to console, debugview by default|Windows, Linux|
|logging_file_sink_win.h, logging_file_sink_win.cpp|Output log to file system|Windows|
|md5.h, md5.cpp|md5 for string, file|Windows, Linux|
|memory_pool.hpp|C++ memeory pool implement|Windows, Linux|
|noncopyable.h|class uncopyable|Windows, Linux|
|os_ver.h, os_ver.cpp|Get Windows system version|Windows|
|pc_info.h, pc_info.cpp|Get compture infomation(IP, MAC, CPU ID)|Windows|
|process_util.h, process_util.cpp|Query, Close, Start process, add UIPI filter|Windows|
|random.h, random.cpp|Generate random number|Windows, Linux|
|registry.h, registry.cpp|Register wirte/read|Windows|
|safe_release_macro.h|Macro：release/close safely|Windows, Linux|
|schedule_task.h, schedule_task.cpp|Add, update, remove windows schedule task|Windows|
|scoped_com_initializer.h, scoped_propvariant.h, scoped_variant.h|COM, PROPVARIANT initialize/uninitialize|Windows|
|scoped_variant.h, scoped_variant.cpp|VARIANT helper|Windows|
|shortcut.h, shortcut.cpp|Create, update, delete shortcut|Windows|
|singleton.h|C++ Singleton|Windows, Linux|
|stack_walker.h, stack_walker.cpp|Get code call stack|Windows|
|string_helper.h, string_helper.cpp|std::string helper, such as find, reserver...|Windows, Linux|
|stringencode.h, stringencode.cpp|string encode|Windows, Linux|
|thread_pool.h|C++ thread pool|Windows, Linux|
|timer.h, timer.cpp|timer|Windows, Linux|
|timeutils.h, timeutils.cpp|time helper|Windows, Linux|
|win_main.h, win_main.cpp|An easy way to generate dump|Windows|
|win_service_base.h, win_service_base.cpp, win_service_installer.h, win_service_installer.cpp|Install, Uninstall, Start, Stop, Puased windows service|Windows|
|ping.h, ping.cpp|Ping implement|Windows|
|host_resolve.h, host_resolve.cpp|Host resolve|Windows|
|networkprotocoldef.h|IP, ICPM protocol header|Windows, Linux|
|ipaddress.h, ipaddress.cpp|IP address|Windows, Linux|
|socketaddress.h, socketaddress.cpp|Socket address|Windows, Linux|
|iocp_socket.h, iocp_socket.cpp, iocpserver.h, iocpserver.cpp|Windows IOCP network model, IOCP server|Windows|
|overlappedsocket.h, overlappedsocket.cpp|Windows Overlapped Socket|Windows|
|miscellaneous.h, miscellaneous.cpp|miscellaneous functions, such as screenshot, version string compare, console colorful progress bar|Windows, Linux|

# 3. Compile
Use `cmake` to generate project appropriate, then compile.

cmake paramenters:

- BUILD_SHARED_LIBS: whether dynamic library or not, `ON`or`OFF`.
- BUILD_TESTS: whether compile test project or not, `ON`or`OFF`.

```bash
# Windows Sample
cmake.exe -G "Visual Studio 15 2017" -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF -S %~dp0 -B %~dp0build
```
