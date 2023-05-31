cd win64/3ddp_host
del *.c,*.cpp,*.h,*.obj,*.dll,*.exp,SConstruct
cd ../3ddp_node
del *.c,*.cpp,*.h,*.obj,*.dll,*.exp,SConstruct
cd ../../
.\init_debug.ps1
.\build.ps1