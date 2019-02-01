# What's ppx?
[ppx](https://github.com/winsoft666/ppx) is a c++ base component library. 
It's most function just be tested on windows platform.

# How To Build
Use Visual Studio 2015 open `vsproject\ppx.sln`, then click Build button.

ppx depend on these third library:
* cryptopp-7.0
* curl-7.63
* openssl-1.1.1
* zlib-1.2.11

However, you don't need to download this library manually, Nuget package will automatic download these.

# ppx On Nuget
After build, Visual Studio will automatic copy output file (`include`, `library`) to `ppx_nuget` folder.
You can find this Nuget package on [https://www.nuget.org/packages/ppx-1.0.0.1-winsoft666/](https://www.nuget.org/packages/ppx-1.0.0.1-winsoft666/).

