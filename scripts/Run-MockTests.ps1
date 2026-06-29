#!/usr/bin/env pwsh

<#
.SYNOPSIS
    运行 CMake 预设中的模拟测试

.PARAMETER TestSuit
    选择要运行哪些测试。可用的取值如下：
        "ME"  / "ModulesExceptions"    模块、启用异常
        "MNE" / "ModulesNoexceptions"  模块、禁用异常
        "HE"  / "HeadersExceptions"    头文件、启用异常
        "HNE" / "HeadersNoexceptions"  头文件、禁用异常

.PARAMETER GeneratorConfig
    CMake 生成器要运行的构建配置，
    用于指定 `cmake --build` 和 `ctest` 的 `--config` 参数

.PARAMETER Quiet
    静默运行，即便运行失败，也不会输出信息
    若传递了该参数，会覆盖掉参数 -OutputOnFailure 的效果

.PARAMETER OutputOnFailure
    仅在运行失败后输出对应的信息；若运行成功，则静默运行，不会输出相关信息

.EXAMPLE
    Run-MockTests.ps1
    以 Release 配置运行所有的预设
.EXAMPLE
    Run-MockTests.ps1 -TestSuit "ME", "MNE"
    以 Release 配置运行“模块+启用异常”和“模块+禁用异常”的测试组合
.EXAMPLE
    Run-MockTests.ps1 -TestSuit "ME", "MNE" -GeneratorConfig Debug
    以 Debug 配置运行“模块+启用异常”和“模块+禁用异常”的测试组合
.EXAMPLE
    Run-MockTests.ps1 -Quiet
    以 Release 配置静默运行所有的预设
.EXAMPLE
    Run-MockTests.ps1 -OutputOnFailure
    以 Release 配置运行所有的预设，仅在失败时输出消息
.EXAMPLE
    Run-MockTests.ps1 -Quiet -OutputOnFailure
    以 Release 配置静默运行所有的预设（-OutputOnFailure 参数的效果被覆盖了）
#>

[CmdletBinding()]
param (
    [ValidateSet("ModulesExceptions", "ModulesNoexceptions", "HeadersExceptions", "HeadersNoexceptions",
        "ME", "MNE", "HE", "HNE"
    )] [string[]]$TestSuit = @("ME", "MNE", "HE", "HNE"),

    [string]$GeneratorConfig = "Release",
    [switch]$Quiet,
    [switch]$OutputOnFailure
)


Write-Debug "`$TestSuit=$TestSuit"

New-Variable -Name suitNameToFullSuitNameMap -Value @{
    "ME"  = "ModulesExceptions"; "ModulesExceptions"  = "ModulesExceptions"
    "MNE" = "ModulesNoexceptions"; "ModulesNoexceptions" = "ModulesNoexceptions"
    "HE"  = "HeadersExceptions"; "HeadersExceptions"  = "HeadersExceptions"
    "HNE" = "HeadersNoexceptions"; "HeadersNoexceptions" = "HeadersNoexceptions"
} -Option ReadOnly


class CMakePreset {
    [string]$Name

    CMakePreset([string]$TestSuitName) {
        $this.Name = "HostTest" + $TestSuitName
    }

    [string] ToString() {
        return $this.Name
    }
}


function Write-Message {
    param (
        [string]$Title,
        [string]$Message,
        [bool]$Succeed
    )

    [scriptblock]$writeAction = {
        $color = if ($Succeed) { [ConsoleColor]::Green } else { [ConsoleColor]::Red }

        Write-Host "`n$("=" * 100)" -ForegroundColor $color
        Write-Host "$Title    $(if ($Succeed) {"PASSED"} else {"FAILED"}), message:" `
            -ForegroundColor $color
        Write-Host $Message
    }

    if (-not $Quiet) {
        if ($OutputOnFailure) {
            if (-not $Succeed) {
                & $writeAction
            }
        }
        else {
            & $writeAction
        }
    }
}


function Invoke-Test {
    [OutputType([int])]
    param ([CMakePreset]$Preset)

    $configure = & cmake --preset "$($Preset.Name)" 2>&1
    $succeed = $LASTEXITCODE -eq 0
    Write-Message -Title "CMake configure with preset `"$($Preset.Name)`"" `
        -Message ($configure | Out-String).Trim() -Succeed $succeed

    if (-not $succeed) { return 1 }

    $build = & cmake --build "build/mock/$($Preset.Name)" --config $GeneratorConfig 2>&1
    $succeed = $LASTEXITCODE -eq 0
    Write-Message -Title "CMake build with preset `"$($Preset.Name)`" on config `"$GeneratorConfig`"" `
        -Message ($build | Out-String).Trim() -Succeed $succeed

    if (-not $succeed) { return 1 }

    $ctestOutput = `
        & ctest --test-dir "build/mock/$($Preset.Name)" -C $GeneratorConfig --output-on-failure 2>&1
    $succeed = $LASTEXITCODE -eq 0
    Write-Message -Title "CTest with preset `"$($Preset.Name)`" on config `"$GeneratorConfig`"" `
        -Message ($ctestOutput | Out-String).Trim() -Succeed $succeed

    if (-not $succeed) { return 1 }

    return 0
}


$beginCwd = (Get-Location).Path
Write-Debug "`$beginCwd=$beginCwd"

$workDirectory = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Write-Debug "`$workDirectory=$workDirectory"

try {
    Set-Location $workDirectory

    $uniqueSuitNames = $TestSuit | ForEach-Object {
        $suitNameToFullSuitNameMap[$_]
    } | Select-Object -Unique

    [CMakePreset[]]$runningPresets = $uniqueSuitNames | ForEach-Object {
        [CMakePreset]::new($_)
    }

    [int]$failedCount = 0
    $runningPresets | ForEach-Object {
        $failedCount += Invoke-Test $_
    }

    if ($failedCount -eq 0) {
        Write-Host "`n        All test(s) PASSED (total $($runningPresets.Count) test(s))" `
            -ForegroundColor Green
    }
    else {
        Write-Host "`n        $failedCount test(s) FAILED (total $($runningPresets.Count) test(s))" `
            -ForegroundColor Red
    }
}
finally {
    Set-Location $beginCwd
}
