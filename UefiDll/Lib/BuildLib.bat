@echo off

call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"

lib /def:UEFIppHost.def /machine:x64 /out:UEFIppHost.lib