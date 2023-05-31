.\setvars.ps1
cd win64/3ddp_host
& $env:scons_path
#scons
#nmake
cd ../3ddp_node
& $env:scons_path
#scons
#nmake