@echo off

REM Change this to your visual studio's 'vcvars64.bat' script path
set MSVC_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
set CXXFLAGS=/std:c++17 /EHsc /W4 /WX /FC /wd4996 /nologo %*

call %MSVC_PATH%\vcvars64.bat

pushd %~dp0
cl %CXXFLAGS% main.cpp /Fe:aoc.exe
popd
