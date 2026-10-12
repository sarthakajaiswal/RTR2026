cl.exe /c /EHsc /I:C:\glew-2.1.0\include OGL.cpp E_shader.cpp 
rc.exe OGL.rc
link.exe OGL.obj E_shader.obj OGL.res /LIBPATH:C:\glew-2.1.0\lib\release\x64 user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
del *.obj 
