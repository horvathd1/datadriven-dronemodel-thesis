cd win64/3ddp_host
del *.c,*.cpp,*.h,*.obj,*.dll,*.exp
cd ../3ddp_node
del *.c,*.cpp,*.h,*.obj,*.dll,*.exp
cd ../../
.\init.ps1
.\build.ps1