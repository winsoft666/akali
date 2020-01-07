[>>> English Version](README.md)

# Akali
C++基础库，不依赖任何第三方库。

---

# 模块
|文件|功能|支持平台|
|---|---|---|
|base64.h <br/> base64.cpp|Base64编解码|Windows <br/> Linux|
|buffer_queue.h <br/> buffer_queue.cpp|基于链表结构的，自动增长的缓冲队列，支持跨链表节点取数据|Windows <br/> Linux|
|byteorder.h <br/> byteorder.cpp|字节序列判断与校验|Windows <br/> Linux|
|constructormagic.h|宏：禁用拷贝构造函数，赋值构造函数|Windows <br/> Linux|
|criticalsection.h <br/> criticalsection.cpp|临界区功能|Windows|
|deprecation.h|接口过时定义|Windows <br/> Linux|
|directory_monitor.h <br/> directory_monitor.cpp|目录监控|Windows|
|display_monitors.h <br/> display_monitors.cpp|获取显示器信息|Windows|
|driver_info.h <br/> driver_info.cpp|获取磁盘驱动器信息|Windows|
|endianess_detect.h <br/> endianess_detect.cpp|大小端检测|Windows <br/> Linux|
|file_info.h <br/> file_info.cpp|获取文件信息(版本，制造商等)|Windows|
|ini.h <br/> ini.cpp|INI配置文件|Windows|
|internet_availability_checker.h <br/> internet_availability_checker.cpp|检测网络是否可用|Windows|
|ipc.h <br/> ipc.cpp|基于命名管道的进程间通信|Windows|
|md5.h <br/> md5.cpp|文件，字符串MD5编码|Windows <br/> Linux|
|memory_pool.hpp|C++内存池实现|Windows <br/> Linux|
|noncopyable.h|类不允许拷贝|Windows <br/> Linux|
|os_ver.h <br/> os_ver.cpp|获取Windows系统版本|Windows|
|pc_info.h <br/> pc_info.cpp|获取计算机信息(IP, MAC, CPU ID)|Windows|
|process_util.h <br/> process_util.cpp|查找，关闭，启动进程，添加UIPI过滤等|Windows|
|process.hpp <br/> process_win.cpp <br/> process_unix.cpp|进程启动、关闭等|Windows <br/> Linux|
|random.h <br/> random.cpp|随机数生成|Windows <br/> Linux|
|registry.h <br/> registry.cpp|注册表读写|Windows|
|macros.h|宏：安全释放等|Windows <br/> Linux|
|schedule_task.h <br/> schedule_task.cpp|Windows计划任务|Windows|
|scoped_com_initializer.h <br/> scoped_propvariant.h <br/> scoped_variant.h|COM, PROPVARIANT初始化、反初始化|Windows|
|scoped_variant.h <br/> scoped_variant.cpp|VARIANT使用|Windows|
|shortcut.h <br/> shortcut.cpp|创建、更新、删除快捷方式|Windows|
|singleton.hpp|单实例类，单实例进程|Windows <br/> Linux|
|stack_walker.h <br/> stack_walker.cpp|获取代码的调用堆栈|Windows|
|string_helper.hpp |std::string辅助函数|Windows <br/> Linux|
|stringencode.h <br/> stringencode.cpp|字符编码|Windows <br/> Linux|
|thread.h <br/> thread.cpp|C++多线程|Windows <br/> Linux|
|thread_pool.h|线程池|Windows <br/> Linux|
|timer.h <br/> timer.cpp|定时器|Windows <br/> Linux|
|timeutils.h <br/> timeutils.cpp|时间获取、转换|Windows <br/> Linux|
|win_main.h <br/> win_main.cpp|dump生成|Windows|
|win_service_base.h <br/> win_service_base.cpp <br/> win_service_installer.h <br/> win_service_installer.cpp|Windows服务安装, 卸载, 启动, 停止|Windows|
|ping.h <br/> ping.cpp|Ping实现|Windows|
|host_resolve.h <br/> host_resolve.cpp|域名解析|Windows|
|networkprotocoldef.h|IP, ICPM网络协议头|Windows <br/> Linux|
|ipaddress.h <br/> ipaddress.cpp|IP地址|Windows <br/> Linux|
|socketaddress.h <br/> socketaddress.cpp|Socket地址|Windows <br/> Linux|
|iocp_socket.h <br/> iocp_socket.cpp <br/> iocpserver.h <br/> iocpserver.cpp|Windows IOCP网络模型及IOCP服务端|Windows|
|overlappedsocket.h <br/> overlappedsocket.cpp|Windows重叠Socket|Windows|
|miscellaneous.h <br/> miscellaneous.cpp|未归类的小功能，如截屏、版本对比、控制台彩色进度等|Windows <br/> Linux|
|filesystem.hpp|类似C++17的filesystem的实现|Windows <br/> Linux|

---

# 编译
使用cmake生成相应工程，然后编译。

cmake参数：

- BUILD_SHARED_LIBS: 是否编译动态库, `ON`或者`OFF`.
- BUILD_TESTS: 是否编译测试工程, `ON`或者`OFF`.

```bash
# Windows示例
cmake.exe -G "Visual Studio 15 2017" -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF -S %~dp0 -B %~dp0build

# Linux
cmake -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF
make
```

---

# 通过vcpkg安装
```bash
vcpkg install akali
```