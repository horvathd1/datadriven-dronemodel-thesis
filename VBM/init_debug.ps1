[xml]$xml=Get-Content FluentDirs.props
$env:FLUENT_BASE=($xml.Project.PropertyGroup | Where-Object Label -eq "UserMacros").FLUENT_BASE
robocopy ./ win64/3ddp_host/ user.txt
robocopy ./ win64/3ddp_host/ SConstruct_debug
Rename-Item -Path "win64/3ddp_host/SConstruct_debug" -NewName "SConstruct"
$file="win64/3ddp_host/user.txt"
(Get-Content user.txt).replace('<CSOURCES>', (ls -Name src/*.cpp,src/*.c) -join ' $(SRC)') |Set-Content $file
(Get-Content $file).replace('<HSOURCES>', (ls -Name src/*.h) -join ' $(SRC)') |Set-Content $file
(Get-Content $file).replace('<VERSION>', "3ddp_host") |Set-Content $file
(Get-Content $file).replace('<PARALLEL>', "none") |Set-Content $file
(Get-Content $file).replace('<FLUENT_BASE>', $env:FLUENT_BASE) |Set-Content $file
(Get-Content $file).replace('<RELEASE>', $env:release) |Set-Content $file
robocopy ./ win64/3ddp_node/ user.txt
robocopy ./ win64/3ddp_node/ SConstruct_debug
Rename-Item -Path "win64/3ddp_node/SConstruct_debug" -NewName "SConstruct"
$file="win64/3ddp_node/user.txt"
(Get-Content user.txt).replace('<CSOURCES>', (ls -Name src/*.cpp,src/*.c) -join ' $(SRC)') |Set-Content $file
(Get-Content $file).replace('<HSOURCES>', (ls -Name src/*.h) -join ' $(SRC)') |Set-Content $file
(Get-Content $file).replace('<VERSION>', "3ddp_node") |Set-Content $file
(Get-Content $file).replace('<FLUENT_BASE>', $env:FLUENT_BASE) |Set-Content $file
(Get-Content $file).replace('<PARALLEL>', "intel") |Set-Content $file
(Get-Content $file).replace('<RELEASE>', $env:release) |Set-Content $file