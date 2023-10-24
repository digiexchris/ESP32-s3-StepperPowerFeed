# Set environment variables
$env:WORKROOT = (Get-Location).Path
$env:IDF_TOOLS_PATH = "$env:WORKROOT\toolchain\lib\bin\espressif"
$env:IDF_PATH = "$env:WORKROOT\toolchain\lib\src\esp-idf"

# Echo out the environment variables
Write-Host "WORKROOT will be set to: $env:WORKROOT"
Write-Host "IDF_TOOLS_PATH will be set to: $env:IDF_TOOLS_PATH"
Write-Host "IDF_PATH will be set to: $env:IDF_PATH"

# Ask for confirmation
$confirmation = Read-Host "Do you want to proceed? (y/n)"
if ($confirmation -eq 'y') {

    # Create directory and navigate into it
    New-Item -Path "$env:WORKROOT\toolchain\lib\src" -ItemType Directory -Force
    Set-Location "$env:WORKROOT\toolchain\lib\src"

    # Clone the esp-idf repository
    git clone -b v4.4.5 --recursive https://github.com/espressif/esp-idf.git

    # Navigate to the esp-idf directory and run the install script
    Set-Location "esp-idf"
    Start-Process -NoNewWindow -FilePath ".\install.bat"
} else {
    Write-Host "Operation cancelled."
}