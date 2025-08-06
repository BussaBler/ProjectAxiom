@echo off

pushd ..
Vendor\Bin\Premake\Windows\premake5.exe --file=Build.lua vs2022
Vendor\Bin\Premake\Windows\premake5.exe --file=Build.lua gmake
popd
pause