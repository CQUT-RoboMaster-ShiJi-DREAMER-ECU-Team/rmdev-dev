#!/usr/bin/env pwsh

<#
.SYNOPSIS
    格式化所有 C++ 代码。直接运行 Format-CppCode.ps1 即可。
#>

[CmdletBinding()]
param ()

$beginCwd = Get-Location
Write-Debug "`$beginCwd=$beginCwd"

$workingDir = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Write-Debug "`$workingDir=$workingDir"

$clangFormatExe = Get-Command clang-format -ErrorAction Stop

Set-Location -Path $workingDir

$formatFailed = $false
try {
    Get-ChildItem -Recurse -Path "emdevif_collection", "rmdev", "tests" `
        -Include "*.c", "*.cpp", "*.h", "*.hpp", "*.cppm" | `
        ForEach-Object {
            & $clangFormatExe -i -style=file $_.FullName 2>&1
            if ($LASTEXITCODE -ne 0) { $global:formatFailed = $true }
        }
}
finally {
    Set-Location -Path $beginCwd
    if ($formatFailed) { exit 1 }
}
