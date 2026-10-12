cl.exe /c /EHsc /Fo:output\ /I:C:\glew-2.1.0\include smoke.cpp imgui_manager.cpp camera.cpp 
rc.exe /fo output\OGL.res OGL.rc
link.exe /OUT:smoke.exe output\*.obj output\OGL.res /LIBPATH:C:\glew-2.1.0\lib\release\x64 user32.lib gdi32.lib assimp-vc143-mt.lib /SUBSYSTEM:WINDOWS

