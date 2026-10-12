cl.exe /c /EHsc /I:C:\glew-2.1.0\include OGL.cpp -diag-suppress 2361
rc.exe OGL.rc
link.exe OGL.obj OGL.res /LIBPATH:C:\glew-2.1.0\lib\release\x64 freetype.lib user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

