[xml]$xml=Get-Content FluentDirs.props
$env:FLUENT_INC=($xml.Project.PropertyGroup | Where-Object Label -eq "UserMacros").FLUENT_BASE
$env:PYTHONHOME=Join-Path -Path $env:FLUENT_INC -ChildPath ../commonfiles/CPython/3_7/winx64/Release/python
$env:PYTHONPATH=Join-Path -Path $env:FLUENT_INC -ChildPath ../commonfiles/CPython/3_7/winx64/Release/python
$env:FLUENT_UDF_COMPILER="clang"
$env:FLUENT_UDF_CLANG="builtin"
$env:scons_path=Join-Path -Path ($xml.Project.PropertyGroup | Where-Object Label -eq "UserMacros").SCONS -ChildPath scons
$env:release= ($xml.Project.PropertyGroup | Where-Object Label -eq "UserMacros").RELEASE