# Set environment variables
$env:WORKROOT = (Get-Location).Path
$env:TOOLCHAIN = "$env:WORKROOT\t"
$env:IDF_TOOLS_PATH = "$env:TOOLCHAIN\espressif"
$env:IDF_PATH = "$env:TOOLCHAIN\esp-idf"


# Echo out the environment variables
Write-Host "Toolchain will be installed to: $env:TOOLCHAIN"
Write-Host "IDF_TOOLS_PATH will be set to: $env:IDF_TOOLS_PATH"
Write-Host "IDF_PATH will be set to: $env:IDF_PATH"
Write-Host "$env:TOOLCHAIN will be cleaned and overwritten"

# Ask for confirmation
$confirmation = Read-Host "Do you want to proceed? (y/n)"
if ($confirmation -eq 'y') {
    Write-Host "Downloading esp-idf-tools-setup-online-2.23.exe to $env:TOOLCHAIN"
    
    # Download a file from a URL
    $url = "https://dl.espressif.com/dl/idf-installer/esp-idf-tools-setup-online-2.23.exe"
    $output = "$env:TOOLCHAIN\esp-idf-tools-setup.exe"
    Invoke-WebRequest -Uri $url -OutFile $output
    Write-Host ""
    Write-Host ""
    Write-Host "******************************************************"
    Write-Host "NOTE!! This project requires version >= 4.4.4 < 5.0.0."
    Write-Host "Please select release/v4.4 when asked for the version."
    Write-Host "******************************************************"
    Write-Host ""
    Write-Host ""
    Write-Host ""

    New-Item -Path "$env:TOOLCHAIN" -ItemType Directory -Force
    Set-Location "$env:TOOLCHAIN"
    ./esp-idf-tools-setup.exe /GITCLEAN=y /IDFDIR="$env:TOOLCHAIN\esp-idf" /IDFVERSION="release/v4.4"

    $InstallerProcess = Get-Process -Name esp-idf-tools-setup
    Wait-Process -Id $InstallerProcess.id

    python.exe $env:IDF_PATH\tools\idf_tools.py install

    Remove-Item -Path $output
    Set-Location "$env:WORKROOT"
    Write-Host ""
    Write-Host ""
    Write-Host ""
    Write-Host "Done. Use $env:WORKROOT\startIdf.ps1 to start an esp-idf terminal."
    Write-Host "Configure the Espressif VSCode extension to use the above tools paths,"
    Write-Host "Optionally saving them to the current workspace."
} else {
    Write-Host "Operation cancelled."
}