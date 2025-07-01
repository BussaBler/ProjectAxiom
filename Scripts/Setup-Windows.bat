@echo off

pushd ..
Vendor\Bin\Premake\Windows\premake5.exe --file=Build.lua vs2022
popd
pause