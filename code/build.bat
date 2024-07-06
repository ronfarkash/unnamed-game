@echo off
mkdir ..\build 2> NUL
pushd ..\build
cl -FC -Zi ..\code\win32_game.cpp user32.lib gdi32.lib
popd 