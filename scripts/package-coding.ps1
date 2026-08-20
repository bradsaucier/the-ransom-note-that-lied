$ErrorActionPreference = "Stop"

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$codingDirectory = Join-Path $repositoryRoot "portfolio\coding"
$workDirectory = Join-Path $repositoryRoot "packaging\.package-work"
$applicationStage = Join-Path $workDirectory "Ransom_Note_That_Lied_Source"
$originalStage = Join-Path $workDirectory "CS405_Original_GoogleTest_Suite"

if (Test-Path -LiteralPath $workDirectory) {
    Remove-Item -Recurse -Force -LiteralPath $workDirectory
}

New-Item -ItemType Directory -Force -Path $applicationStage, $originalStage | Out-Null

$applicationItems = @(
    "CMakeLists.txt",
    "include",
    "src",
    "tests",
    "samples",
    "README.md",
    "LICENSE"
)

foreach ($item in $applicationItems) {
    Copy-Item -Recurse -Force -LiteralPath (Join-Path $repositoryRoot $item) -Destination $applicationStage
}

Copy-Item -Force -LiteralPath (Join-Path $repositoryRoot "packaging\original-google-test\test.cpp") -Destination $originalStage
Copy-Item -Force -LiteralPath (Join-Path $repositoryRoot "packaging\original-google-test\README.txt") -Destination $originalStage

$applicationArchive = Join-Path $codingDirectory "Ransom_Note_That_Lied_Source.zip"
$originalArchive = Join-Path $codingDirectory "CS405_Original_GoogleTest_Suite.zip"

if (Test-Path -LiteralPath $applicationArchive) {
    Remove-Item -Force -LiteralPath $applicationArchive
}
if (Test-Path -LiteralPath $originalArchive) {
    Remove-Item -Force -LiteralPath $originalArchive
}

Compress-Archive -Path (Join-Path $applicationStage "*") -DestinationPath $applicationArchive -CompressionLevel Optimal
Compress-Archive -Path (Join-Path $originalStage "*") -DestinationPath $originalArchive -CompressionLevel Optimal

Remove-Item -Recurse -Force -LiteralPath $workDirectory

Write-Host "Created coding archives in $codingDirectory"
