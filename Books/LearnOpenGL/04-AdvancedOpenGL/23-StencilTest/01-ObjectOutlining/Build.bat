cl.exe /c /EHsc /I:C:\glew-2.1.0\include *.cpp /Fo:"output/"
rc.exe OGL.rc 
link.exe /OUT:OGL.exe output/*.obj OGL.res /LIBPATH:C:\glew-2.1.0\lib\release\x64 user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

