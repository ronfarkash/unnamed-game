@echo off
mkdir ..\build 2> NUL
pushd ..\build
cl -DUNNAMED_INTERNAL=1 -DUNNAMED_SLOW=1 -FC -Zi ..\code\win32_unnamed.cpp user32.lib gdi32.lib
popd 