echo off
echo /p:Configuration=Debug;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=Debug;Platform=x86

echo /p:Configuration=Debug_Min;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=Debug_Min;Platform=x86

echo /p:Configuration=DebugHTTP;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=DebugHTTP;Platform=x86

echo /p:Configuration=DebugDLL;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=DebugDLL;Platform=x86

echo /p:Configuration=DebugDLL_Min;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=DebugDLL_Min;Platform=x86

echo /p:Configuration=DebugDLLHTTP;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=DebugDLLHTTP;Platform=x86

echo /p:Configuration=Release;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=Release;Platform=x86

echo /p:Configuration=Release_Min;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=Release_Min;Platform=x86

echo /p:Configuration=ReleaseHTTP;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=ReleaseHTTP;Platform=x86

echo /p:Configuration=ReleaseDLL;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=ReleaseDLL;Platform=x86

echo /p:Configuration=ReleaseDLL_Min;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=ReleaseDLL_Min;Platform=x86

echo /p:Configuration=ReleaseDLLHTTP;Platform=x86
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" -consoleloggerparameters:ErrorsOnly "%~dp0\ppx.vcxproj" /t:Rebuild /p:Configuration=ReleaseDLLHTTP;Platform=x86
pause
