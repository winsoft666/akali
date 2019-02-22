# 1. ppx是什么
[ppx](https://github.com/winsoft666/ppx) 是一个C++的基础组件库，其提供了在Windows平台进行应用层开发所需要的大多数功能，其提供的大部分功能在Windows平台下通过了产品的验证和测试。

# 如何编译
只需要2步：使用Visual Stuido 2015打开`vsproject\ppx.sln`，然后选择对应的编译选项进行编译即可。


ppx 依赖如下的第三方库:
* cryptopp-7.0
* curl-7.63
* openssl-1.1.1
* zlib-1.2.11

但是，这些库并不需要你手动去下载配置。ppx使用了Visual Studio提供的Nuget包管理器，在`packages.config`文件中配置了这些库的版本号，Nuget包管理器在编译的时候会自动下载配置这些依赖库。

# 2. ppx包含哪些功能
## 2.1 断言
`base\assert.h`

## 2.2 Base64编解码
`base\base64.h`

## 2.3 Md5
`base\md5.h`
支持字符串MD5，文件/大文件MD5

## 2.4 命令行参数定义、解析
`base\flags.h` 
命令行参数定义，使用方法可以参考：[https://blog.csdn.net/china_jeffery/article/details/78653397](https://blog.csdn.net/china_jeffery/article/details/78653397)

`base\cmdline_parse.h`
命令行参数解析

## 2.5 字节序列转换、大小端
`base\byteorder.h`
`base\endianess_detect.h`

## 2.6 临界区、事件
`base\criticalsection.h`
`base\event.h`

## 2.7 磁盘驱动器信息获取
`base\driver_info.h`

## 2.8 文件信息获取
`base\file_info.h`

## 2.9 文件/目录读写、删除、拷贝等操作
`base\file_util.h`

## 2.10 日志
`base\logging.h`
`base\logging_file_sink_win.h`

## 2.11 内存池
`base\memory_pool.hpp`

## 2.12 支持跨MT模块安全传递的std::string
`base\module_safe_string.h`

## 2.13 操作系统版本获取和判断
`base\os_ver.h`

## 2.14 本地计算机信息获取
`base\pc_info.h`

## 2.15 安全释放宏
`base\safe_release_macro.h`

## 2.16 单实例
`base\singleton.h`

## 2.17 字符串
`base\string.h`

## 2.18 字符编码
`base\stringencode.h`
支持ANIS、Unicode、Utf8、Utf-BOM 这几种编码间的相互转换

字符编码参考：[拨开字符编码的迷雾](https://blog.csdn.net/china_jeffery/column/info/18195)

## 2.19 线程辅助函数
`base\thread_util.h`

## 2.20 进程辅助函数
`base\process_util.h`

## 2.21 定时器
`base\timer.h`

## 2.22 Dump生成
`base\win_main.h`
使用2行代码即可方便的让程序拥有生成崩溃dump的功能：
```c++
//用WINMAIN_BEGIN, WINMAIN_END取代WinMain，用MAIN_BEGIN, MAIN_END取代main
//
WINMAIN_BEGIN(TEXT("Test"))
WINMAIN_END
```

## 2.23 Windows服务
`base\win_service_base.h`
`base\win_service_installer.h`

## 2.24 随机数生成
`base\random.h`

## 2.25 AES加解密
`encrypt\encrypt_helper.h`

## 2.26 RES加解密
`encrypt\encrypt_helper.h`

## 2.27 文件下载
`net\file_download.h`
支持多线程下载、断点续传

## 2.28 HTTP请求
`net\http_request.h`

## 2.29 Ping功能
`net\ping.h`

## 2.30 IP、Socket封装
`net\ipaddress.h`
`net\socket.h`
`net\socketaddress.h`

## 2.31 Host解析
`net\host_resolve.h`

## 2.32 IOCP及IOCP Server
`net\iocp.h`
`net\iocpserver.h`

Windows网络模型可以参考：[Windows网络模型](https://blog.csdn.net/china_jeffery/column/info/19222)

## 2.33 Callback对象
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

## 2.34 时间获取转换等操作
`base\timeutil.h`

# 3. 发布ppx到Nuget
在编译完成以后，Visual Studio会通过生成后事件自动将头文件、库文件拷贝到`ppx_nuget`目录，通过该目录中的文件生成nupkg文件之后就可以发布到Nuget平台了。

目前ppx已经发布到了Nuget，见：[https://www.nuget.org/packages/ppx-1.0.0.1-winsoft666/](https://www.nuget.org/packages/ppx-1.0.0.1-winsoft666/)


**感谢您的使用，欢迎提交BUG！**