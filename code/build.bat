@echo off
mkdir ..\build 2> NUL
pushd ..\build
cl -MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -DUNNAMED_INTERNAL=1 -DUNNAMED_SLOW=1 -FC -Z7 -Fmwin32_unnamed.map ..\code\win32_unnamed.cpp /link -subsystem:windows,5.1 user32.lib gdi32.lib
popd 