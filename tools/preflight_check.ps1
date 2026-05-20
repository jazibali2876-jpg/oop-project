#requires -Version 5.1
<#
    Smart Restaurant POS — preflight check.

    Usage:
        powershell -ExecutionPolicy Bypass -File tools\preflight_check.ps1
        powershell -ExecutionPolicy Bypass -File tools\preflight_check.ps1 -Target dist\smart_pos.exe

    Validates: compiler, SFML linkage, runtime DLL presence next to the .exe,
    font files, asset/data paths. Exits non-zero on any FAIL.
#>

param(
    [string] $Target = ""
)

$ErrorActionPreference = "Continue"

# Project root = parent of this script's folder.
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $ProjectRoot

$Mingw = Join-Path $ProjectRoot "mingw64\bin"
$Sfml  = Join-Path $ProjectRoot "third_party\SFML-mingw"

# When -Target is not given, default to the freshest smart_pos.exe under dist/.
if (-not $Target) {
    $Target = Join-Path $ProjectRoot "dist\smart_pos.exe"
}
$TargetDir = Split-Path -Parent $Target

$results = @()
function Add-Result($name, $ok, $detail) {
    if ($ok) { $status = "PASS" } else { $status = "FAIL" }
    $script:results += [pscustomobject]@{ Check = $name; Status = $status; Detail = $detail }
}

# ---- 1. Compiler ---------------------------------------------------------
$gpp = Join-Path $Mingw "g++.exe"
if (Test-Path $gpp) {
    $ver = (& $gpp --version | Select-Object -First 1)
    Add-Result "Compiler (vendored MinGW g++)" $true $ver
} else {
    Add-Result "Compiler (vendored MinGW g++)" $false "missing $gpp"
}

# ---- 2. SFML linkage (compile the link probe) ----------------------------
$probe = Join-Path $ProjectRoot "tools\probes\sfml_probe.cpp"
$probeOut = Join-Path $ProjectRoot "build\probe\sfml_probe.exe"
if ((Test-Path $probe) -and (Test-Path $gpp)) {
    $sfmlInc = Join-Path $Sfml "include"
    $sfmlLib = Join-Path $Sfml "lib"
    if ((Test-Path $sfmlInc) -and (Test-Path $sfmlLib)) {
        $null = New-Item -ItemType Directory -Force -Path (Split-Path $probeOut) | Out-Null
        $args = @(
            "-std=c++17", $probe,
            "-I", $sfmlInc, "-L", $sfmlLib,
            "-lsfml-graphics", "-lsfml-window", "-lsfml-system",
            "-o", $probeOut
        )
        $proc = & $gpp @args 2>&1
        if ($LASTEXITCODE -eq 0 -and (Test-Path $probeOut)) {
            Add-Result "SFML linkage (MinGW link of probe)" $true "linked: $probeOut"
        } else {
            Add-Result "SFML linkage (MinGW link of probe)" $false ([string]::Join("`n", $proc))
        }
    } else {
        Add-Result "SFML linkage (MinGW link of probe)" $false "missing SFML include or lib under $Sfml"
    }
} else {
    Add-Result "SFML linkage (MinGW link of probe)" $false "probe or compiler missing"
}

# ---- 3. Runtime DLLs next to target --------------------------------------
$dllChecks = @(
    "sfml-graphics-3.dll",
    "sfml-window-3.dll",
    "sfml-system-3.dll",
    "sfml-audio-3.dll",
    "libgcc_s_seh-1.dll",
    "libstdc++-6.dll",
    "libwinpthread-1.dll"
)
if (Test-Path $TargetDir) {
    foreach ($d in $dllChecks) {
        $p = Join-Path $TargetDir $d
        if (Test-Path $p) {
            Add-Result "DLL: $d" $true "found at $p"
        } else {
            Add-Result "DLL: $d" $false "MISSING next to $Target"
        }
    }
} else {
    foreach ($d in $dllChecks) {
        Add-Result "DLL: $d" $false "target dir does not exist: $TargetDir"
    }
}

# ---- 4. Fonts ------------------------------------------------------------
$fontDir = Join-Path $ProjectRoot "assets\fonts"
foreach ($f in @("Inter-Regular.ttf", "Inter-Bold.ttf", "fa-solid-900.ttf")) {
    $p = Join-Path $fontDir $f
    if (Test-Path $p) {
        $size = (Get-Item $p).Length
        Add-Result "Font: $f" $true "$size bytes"
    } else {
        Add-Result "Font: $f" $false "missing $p"
    }
}

# Mirror fonts next to the target too (so the .exe finds them after packaging).
if (Test-Path $TargetDir) {
    $distFontDir = Join-Path $TargetDir "assets\fonts"
    foreach ($f in @("Inter-Regular.ttf", "Inter-Bold.ttf", "fa-solid-900.ttf")) {
        $p = Join-Path $distFontDir $f
        if (Test-Path $p) {
            Add-Result "Packaged font: $f" $true "in dist"
        } else {
            Add-Result "Packaged font: $f" $false "not yet in dist (OK pre-packaging)"
        }
    }
}

# ---- 5. Asset/data paths -------------------------------------------------
foreach ($d in @("assets", "assets\fonts", "assets\img", "data")) {
    $p = Join-Path $ProjectRoot $d
    if (Test-Path $p) {
        Add-Result "Path: $d" $true $p
    } else {
        Add-Result "Path: $d" $false "missing $p"
    }
}

# ---- Report --------------------------------------------------------------
Write-Host ""
Write-Host "============================================================"
Write-Host "  Smart Restaurant POS - Preflight Check"
Write-Host "  Project root: $ProjectRoot"
Write-Host "  Target:       $Target"
Write-Host "============================================================"
$pass = ($results | Where-Object { $_.Status -eq "PASS" }).Count
$fail = ($results | Where-Object { $_.Status -eq "FAIL" }).Count
foreach ($r in $results) {
    $color = if ($r.Status -eq "PASS") { "Green" } else { "Red" }
    Write-Host ("  [{0}] {1,-40} {2}" -f $r.Status, $r.Check, $r.Detail) -ForegroundColor $color
}
Write-Host "------------------------------------------------------------"
Write-Host ("  Total: {0}   Pass: {1}   Fail: {2}" -f ($pass + $fail), $pass, $fail)
Write-Host "============================================================"

if ($fail -gt 0) {
    exit 1
}
exit 0
