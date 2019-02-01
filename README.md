# ppx是什么
[ppx](https://github.com/winsoft666/ppx) 是一个C++的基础组件库，其提供了在Windows平台进行应用层开发所需要的大多数功能，其提供的大部分功能在Windows平台下通过了产品的验证和测试。

# 如何编译
只需要2步：使用Visual Stuido 2015打开`vsproject\ppx.sln`，然后选择对应的编译选项进行编译即可。


ppx 依赖如下的第三方库:
* cryptopp-7.0
* curl-7.63
* openssl-1.1.1
* zlib-1.2.11

但是，这些库并不需要你手动去下载配置。ppx使用了Visual Studio提供的Nuget包管理器，在`packages.config`文件中配置了这些库的版本号，Nuget包管理器在编译的时候会自动下载配置这些依赖库。


# 发布ppx到Nuget
在编译完成以后，Visual Studio会通过生成后事件自动将头文件、库文件拷贝到`ppx_nuget`目录，通过该目录中的文件生成nupkg文件之后就可以发布到Nuget平台了。

目前ppx已经发布到了Nuget，见：[https://www.nuget.org/packages/ppx-1.0.0.1-winsoft666/](https://www.nuget.org/packages/ppx-1.0.0.1-winsoft666/)


新年快乐, 2019！