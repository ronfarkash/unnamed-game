@echo off

set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -DUNNAMED_INTERNAL=1 -DUNNAMED_SLOW=1 -FC -Z7 -Fmwin32_unnamed.map
set CommonLinkerFlags=-opt:ref user32.lib gdi32.lib winmm.lib

mkdir ..\build 2> NUL
pushd ..\build
cl  %CommonCompilerFlags% ..\code\win32_unnamed.cpp /link %CommonLinkerFlags%
popd 