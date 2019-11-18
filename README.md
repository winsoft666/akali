# 1. What's ppxbase
[ppxbase](https://github.com/winsoft666/ppxbase) is a C++ common library.

# Update run_cmake.bat
CMAKE_TOOLCHAIN_FILE: `***\scripts\buildsystems\vcpkg.cmake` file path.
VCPKG_TARGET_TRIPLET: static library is `x86-windows-static`, dynamic library is `x86-windows`.
BUILD_SHARED_LIBS: whether compile dynamic library, `ON`or`OFF`.

# 2. Feature
## 2.1 Assert
`base\assert.h`

## 2.2 Base64
`base\base64.h`

## 2.3 Md5
`base\md5.h`
String MD5，File/Big file MD5

## 2.4 Command line parse
`base\flags.h` 
[https://blog.csdn.net/china_jeffery/article/details/78653397](https://blog.csdn.net/china_jeffery/article/details/78653397)

`base\cmdline_parse.h`
命令行参数解析

## 2.5 Byte order
`base\byteorder.h`
`base\endianess_detect.h`

## 2.6 Critical Section/Event
`base\criticalsection.h`
`base\event.h`

## 2.7 Disk driver information
`base\driver_info.h`

## 2.8 File information
`base\file_info.h`

## 2.9 File operation
`base\file_util.h`

## 2.10 File Log
`base\logging.h`
`base\logging_file_sink_win.h`

## 2.11 Memory Pool
`base\memory_pool.hpp`

See: (https://blog.csdn.net/china_jeffery/article/details/80750042)

## 2.12 How to pass std::string parameters cross MT module?
`base\module_safe_string.h`

See:
[Windows内存体系（9） -- 跨MT模块的内存相互释放的问题](https://blog.csdn.net/china_jeffery/article/details/79656307)
[Windows内存体系（10） -- 使用std::string跨MT模块传参](https://blog.csdn.net/china_jeffery/article/details/79725667)

## 2.13 OS version
`base\os_ver.h`

## 2.14 PC information
`base\pc_info.h`

## 2.15 Safe release/delete/close macro
`base\safe_release_macro.h`

## 2.16 Singleton
`base\singleton.h`

## 2.17 String Util
`base\string.h`

## 2.18 String Encode
`base\stringencode.h`

See: [拨开字符编码的迷雾](https://blog.csdn.net/china_jeffery/column/info/18195)

## 2.19 Thread Util
`base\thread_util.h`

## 2.20 Process Util
`base\process_util.h`

## 2.21 Timer
`base\timer.h`

## 2.22 Dump Util
`base\win_main.h`

How to use 2 line code to generate dump file:
```c++
//用WINMAIN_BEGIN, WINMAIN_END取代WinMain，用MAIN_BEGIN, MAIN_END取代main
//
WINMAIN_BEGIN(TEXT("Test"))
WINMAIN_END
```

Other way to generate dump: [如何生成转储(dmp)文件--工具篇](https://blog.csdn.net/china_jeffery/article/details/78412415)

## 2.23 Windows Service
`base\win_service_base.h`
`base\win_service_installer.h`

## 2.24 Random
`base\random.h`

## 2.25 AES/DES
`encrypt\encrypt_helper.h`

## 2.26 RES
`encrypt\encrypt_helper.h`

## 2.27 Callback对象
`base\bind.h`
`base\callback.h`

支持将任意函数（不限于参数个数、类型、返回值）封装为对象，利用Callback对象可以实现类似cef中的向指定线程post task的功能：
```c++
// PostTaskToUIThread的定义见：https://github.com/winsoft666/duilib
//
void DlgMain::OnNeedManualEnableAero() {
    PostTaskToUIThread(base::BindLambda([this]() {
		// ...
    }));
}

```

## 2.28 时间获取转换等操作
`base\timeutil.h`

## 2.29 Windows IPC based Named Pipe
`base\ipc.h`


---------------------------------------------------------------

**感谢您的使用，欢迎提交BUG！**
