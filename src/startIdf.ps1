$env:WORKROOT = (Get-Location).Path
$env:TOOLCHAIN = "$env:WORKROOT\toolchain"
$env:IDF_TOOLS_PATH = "$env:TOOLCHAIN\lib\bin\espressif"
$env:IDF_PATH = "$env:TOOLCHAIN\lib\src\esp-idf"
# $env:PYTHONHOME = "$env:IDF_TOOLS_PATH\python_env\idf4.4_py3.11_env\"
# $env:PYTHONPATH = "$env:IDF_TOOLS_PATH\python_env\idf4.4_py3.11_env\"
# $env:Path = "$env:IDF_TOOLS_PATH\python_env\idf4.4_py3.11_env\Scripts\;" + $env:Path
#https://www.python.org/ftp/python/3.11.2/python-3.11.2-embed-amd64.zip

Write-Host $env:Path

Write-Host "Python env is $env:PYTHON"
# python.exe -v
#D:\Machining-tools\StepperPowerFeed\src\toolchain\lib\bin\espressif\tools\idf-python\3.11.2\python.exe -v
# Echo out the environment variables
Write-Host "Toolchain is in to: $env:TOOLCHAIN"
Write-Host "IDF_TOOLS_PATH will be set to: $env:IDF_TOOLS_PATH"
Write-Host "IDF_PATH will be set to: $env:IDF_PATH"

. $env:IDF_PATH/export.ps1