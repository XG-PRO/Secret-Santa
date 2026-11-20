@echo off
rem Needs g++ (winget install BrechtSanders.WinLibs.POSIX.UCRT). Output: "..\Secret Santa.exe"
cd /d "%~dp0"
windres app.rc -O coff -o app.res && ^
g++ -std=c++20 -O2 -Wall -static -s -municode -mwindows -D_WIN32_WINNT=0x0A00 main.cpp app.res -o "..\Secret Santa.exe" -lbcrypt -lcomctl32 -lole32 -luuid -lshell32
del app.res
