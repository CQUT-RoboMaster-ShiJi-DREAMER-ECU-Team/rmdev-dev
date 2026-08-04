#!/usr/bin/env pwsh

<#
.SYNOPSIS
    同步本仓库的修改到 subtrees 中

.DESCRIPTION
    将本仓库的修改同步到 subtrees 中，使用 `git subtree push` 命令。
    该脚本会将本仓库的修改推送到以下子树中：
        - rmdev
        - emdevif_collection/emdevif
        - emdevif_collection/emdevif_stm32_peripheral
    后续如果有新的子树，也需要同步更新这个脚本

.PARAMETER Ssh
    使用 SSH 协议进行同步，而不是默认的 HTTPS 协议

.PARAMETER UrlBaseOverride
    覆盖默认的 URL 基础路径，指定到用户级（通常用于 ssh 链接的覆盖，例如
    `git@user.github.com:CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team`，不要指定到具体的仓库链接）
#>

[CmdletBinding()]
param(
    [switch]$Ssh,
    [string]$UrlBaseOverride = $null
)


function Push-Subtree {
    param(
        [string]$Prefix,
        [string]$Url,
        [string]$Branch = "main"
    )

    git subtree push --prefix="$Prefix" "$Url" "$Branch"
}


New-Variable -Name defaultHttpBase `
    -Value "https://github.com/CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team" `
    -Option ReadOnly
New-Variable -Name defaultSshBase `
    -Value "git@github.com:CQUT-RoboMaster-ShiJi-DREAMER-ECU-Team" `
    -Option ReadOnly

$beginCwd = (Get-Location).Path
Write-Debug "`$beginCwd=$beginCwd"

$workDirectory = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Write-Debug "`$workDirectory=$workDirectory"

Set-Location $workDirectory
try {
    $urlBase = $null
    if ($UrlBaseOverride) {
        $urlBase = $UrlBaseOverride
    } else {
        $urlBase = if ($Ssh) { $defaultSshBase } else { $defaultHttpBase }
    }

    Push-Subtree -Prefix "rmdev" -Url "$urlBase/rmdev"
    Push-Subtree -Prefix "emdevif_collection/emdevif" -Url "$urlBase/emdevif"
    Push-Subtree -Prefix "emdevif_collection/emdevif_stm32_peripheral" -Url "$urlBase/emdevif_stm32_peripheral"
} finally {
    Set-Location $beginCwd
}
