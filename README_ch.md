# 1. 项目介绍
[ppxbase](https://github.com/winsoft666/ppxbase)是一个通用的C++功能库，不依赖任何第三方库。

# 2. 功能模块
|文件|功能|支持平台|
|---|---|---|
|assert.h, assert.cpp|运行时断言，支持附加消息|Windows, Linux|
|base64.h, base64.cpp|Base64编解码|Windows, Linux|
|buffer_queue.h, buffer_queue.cpp|基于链表结构的，自动增长的缓冲队列，支持跨链表节点取数据|Windows, Linux|
|byteorder.h, byteorder.cpp|字节序列判断与校验|Windows, Linux|
|cmdline_parse.h, cmdline_parse.cpp|命令行参数解析|Windows, Linux|
|flags.h, flags.cpp|命令行参数定义，解析|Windows, Linux|
|constructormagic.h|宏：禁用拷贝构造函数，赋值构造函数|Windows, Linux|
|criticalsection.h, criticalsection.cpp|临界区功能|Windows|
|deprecation.h|接口过时定义|Windows, Linux|
|directory_monitor.h, directory_monitor.cpp|目录监控|Windows|
|display_monitors.h, display_monitors.cpp|获取显示器信息|Windows|
|driver_info.h, driver_info.cpp|获取磁盘驱动器信息|Windows|
|endianess_detect.h, endianess_detect.cpp|大小端检测|Windows, Linux|
|file_info.h, file_info.cpp|获取文件信息(版本，制造商等)|Windows|
|ini.h, ini.cpp|INI配置文件|Windows|
|internet_availability_checker.h, internet_availability_checker.cpp|检测网络是否可用|Windows|
|ipc.h, ipc.cpp|基于命名管道的进程间通信|Windows|
|logging.h, logging.cpp|日志|Windows, Linux|
|logging_file_sink_win.h, logging_file_sink_win.cpp|结合logging将日志输出到文件|Windows|
|md5.h, md5.cpp|文件，字符串MD5编码|Windows, Linux|
|memory_pool.hpp|C++内存池实现|Windows, Linux|
|noncopyable.h|类不允许拷贝|Windows, Linux|
|os_ver.h, os_ver.cpp|获取Windows系统版本|Windows|
|pc_info.h, pc_info.cpp|获取计算机信息(IP, MAC, CPU ID)|Windows|
|process_util.h, process_util.cpp|查找，关闭，启动进程，关闭进程，添加UIPI过滤等|Windows|
|random.h, random.cpp|随机数生成|Windows, Linux|
|registry.h, registry.cpp|注册表读写|Windows|
|safe_release_macro.h|宏：安全释放|Windows, Linux|
|schedule_task.h, schedule_task.cpp|Windows计划任务|Windows|
|scoped_com_initializer.h, scoped_propvariant.h, scoped_variant.h|COM, PROPVARIANT初始化、反初始化|Windows|
|scoped_variant.h, scoped_variant.cpp|VARIANT使用|Windows|
|shortcut.h, shortcut.cpp|创建、更新、删除快捷方式|Windows|
|singleton.h|单实例|Windows, Linux|
|stack_walker.h, stack_walker.cpp|获取代码的调用堆栈|Windows|
|string_helper.h, string_helper.cpp|std::string辅助函数，如查找、截取等|Windows, Linux|
|stringencode.h, stringencode.cpp|字符编码|Windows, Linux|
|thread_pool.h|线程池|Windows, Linux|
|timer.h, timer.cpp|定时器|Windows, Linux|
|timeutils.h, timeutils.cpp|时间获取、转换|Windows, Linux|
|win_main.h, win_main.cpp|dump生成|Windows|
|win_service_base.h, win_service_base.cpp, win_service_installer.h, win_service_installer.cpp|Windows服务安装, 卸载, 启动, 停止|Windows|
|ping.h, ping.cpp|Ping实现|Windows|
|host_resolve.h, host_resolve.cpp|域名解析|Windows|
|networkprotocoldef.h|IP, ICPM网络协议头|Windows, Linux|
|ipaddress.h, ipaddress.cpp|IP地址|Windows, Linux|
|socketaddress.h, socketaddress.cpp|Socket地址|Windows, Linux|
|iocp_socket.h, iocp_socket.cpp, iocpserver.h, iocpserver.cpp|Windows IOCP网络模型及IOCP服务端|Windows|
|overlappedsocket.h, overlappedsocket.cpp|Windows重叠Socket|Windows|
|miscellaneous.h, miscellaneous.cpp|未归类的小功能，如截屏、版本对比、控制台彩色进度等|Windows, Linux|

# 3. 编译
使用cmake生成相应工程，然后编译。

cmake参数：

- BUILD_SHARED_LIBS: 是否编译动态库, `ON`or`OFF`.
- BUILD_TESTS: 是否编译测试工程, `ON`or`OFF`.

```bash
# Windows示例
cmake.exe -G "Visual Studio 15 2017" -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF -S %~dp0 -B %~dp0build
```