cl.exe /c /EHsc OGL2.c 

rc.exe OGL.rc 

link.exe OGL2.obj OGL.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS 
