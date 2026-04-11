[CmdletBinding()]
param(
    [string]$VendorExportPath = '',
    [string]$ServerCsvDir = '\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg\bin\csv'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$scriptRoot = if ($PSScriptRoot) { $PSScriptRoot } else { Split-Path -Parent $MyInvocation.MyCommand.Path }
. (Join-Path $scriptRoot 'VendorPropIdRules.ps1')

if ([string]::IsNullOrWhiteSpace($VendorExportPath)) {
    $VendorExportPath = Join-Path $scriptRoot '..\doc\VendorPropExport.csv'
}

function Get-ServerColor {
    param([string]$Rarity)

    switch ($Rarity) {
        'Common' { return 1 }
        'Superior' { return 2 }
        'Epic' { return 3 }
        'Legendary' { return 4 }
        default { return 1 }
    }
}

function Get-ServerEquipKind {
    param(
        [string]$ItemType,
        [string]$Slot
    )

    if ((Normalize-VendorItemType $ItemType) -in @('Weapon', 'Shield', 'Bow', 'Arrow')) { return 1 }

    switch ($Slot) {
        'Head' { return 2 }
        'Chestplate' { return 3 }
        'Gloves' { return 3 }
        'Legs' { return 3 }
        'Neck' { return 4 }
        'Ring' { return 5 }
        'Bracelet' { return 6 }
        'Talisman' { return 6 }
        'Boots' { return 7 }
        default { return 3 }
    }
}

function Get-ServerEquipPart {
    param(
        [string]$ItemType,
        [string]$Slot
    )

    if ((Normalize-VendorItemType $ItemType) -in @('Weapon', 'Shield', 'Bow', 'Arrow')) { return 0 }

    switch ($Slot) {
        'Head' { return 1 }
        'Chestplate' { return 2 }
        'Gloves' { return 2 }
        'Legs' { return 2 }
        'Neck' { return 3 }
        'Ring' { return 4 }
        'Bracelet' { return 6 }
        'Talisman' { return 6 }
        'Boots' { return 8 }
        default { return 2 }
    }
}

function Parse-PositiveInt {
    param(
        [string]$ValueText,
        [int]$DefaultValue
    )

    $parsed = 0
    if ([int]::TryParse($ValueText, [ref]$parsed) -and $parsed -gt 0) {
        return $parsed
    }
    return $DefaultValue
}

function Write-PlainCsv {
    param(
        [string]$Path,
        [string[]]$Headers,
        [object[]]$Rows
    )

    $lines = New-Object System.Collections.Generic.List[string]
    $lines.Add(($Headers -join ','))

    foreach ($row in $Rows) {
        $values = foreach ($header in $Headers) {
            $text = [string]$row.$header
            $text = $text -replace ',', '_'
            $text
        }
        $lines.Add(($values -join ','))
    }

    [System.IO.File]::WriteAllLines($Path, $lines, [System.Text.UTF8Encoding]::new($false))
}

function Ensure-UniquePropNick {
    param([object[]]$Rows)

    $counts = @{}
    foreach ($row in $Rows) {
        $nick = [string]$row.nick
        if ([string]::IsNullOrWhiteSpace($nick) -or $nick.Contains([string][char]0xFFFD)) {
            $nick = ('legacy_{0}' -f [string]$row.id)
        }

        if ($counts.ContainsKey($nick)) {
            $counts[$nick]++
        } else {
            $counts[$nick] = 1
        }
    }

    $usedNick = New-Object 'System.Collections.Generic.HashSet[string]'
    foreach ($row in $Rows) {
        $nick = [string]$row.nick
        if ([string]::IsNullOrWhiteSpace($nick) -or $nick.Contains([string][char]0xFFFD)) {
            $nick = ('legacy_{0}' -f [string]$row.id)
        }

        $candidate = if ($counts[$nick] -gt 1) {
            ('{0}__{1}' -f $nick, [string]$row.id)
        } else {
            $nick
        }

        while ($usedNick.Contains($candidate)) {
            $candidate = ('{0}_dup' -f $candidate)
        }

        $row.nick = $candidate
        [void]$usedNick.Add($candidate)
    }

    return $Rows
}

$vendorRows = @(Import-Csv $VendorExportPath | Sort-Object source_table, row_name)
if ($vendorRows.Count -eq 0) {
    throw "No vendor rows found in $VendorExportPath"
}

$duplicateKeyGroups = @(
    $vendorRows |
        Group-Object { Get-VendorUniqueItemKey -SourceTable ([string]$_.source_table) -RowName ([string]$_.row_name) } |
        Where-Object { $_.Count -gt 1 }
)
if ($duplicateKeyGroups.Count -gt 0) {
    throw ("Duplicate source_table + row_name keys found in vendor export: {0}" -f ($duplicateKeyGroups[0].Name))
}

$duplicatePropIds = @(
    $vendorRows |
        Group-Object server_propid |
        Where-Object { -not [string]::IsNullOrWhiteSpace($_.Name) -and $_.Count -gt 1 }
)
if ($duplicatePropIds.Count -gt 0) {
    throw ("Duplicate server_propid found in vendor export: {0}" -f ($duplicatePropIds[0].Name))
}

$propCsvPath = Join-Path $ServerCsvDir 'prop.csv'
$equipCsvPath = Join-Path $ServerCsvDir 'equip.csv'
$consumeCsvPath = Join-Path $ServerCsvDir 'consume.csv'

$propRows = @(Import-Csv $propCsvPath)
$equipRows = @(Import-Csv $equipCsvPath)
$consumeRows = @(Import-Csv $consumeCsvPath)

$targetPropIds = New-Object 'System.Collections.Generic.HashSet[string]'
$targetRowNames = New-Object 'System.Collections.Generic.HashSet[string]'
$targetItemIds = New-Object 'System.Collections.Generic.HashSet[string]'
$targetServerNicks = New-Object 'System.Collections.Generic.HashSet[string]'

foreach ($row in $vendorRows) {
    [void]$targetPropIds.Add([string]$row.server_propid)
    [void]$targetRowNames.Add([string]$row.row_name)
    [void]$targetItemIds.Add([string]$row.item_id)
    [void]$targetServerNicks.Add((Get-VendorServerNick -SourceTable ([string]$row.source_table) -RowName ([string]$row.row_name)))
}

$rowsToRemoveFromProp = @(
    $propRows | Where-Object {
        $targetPropIds.Contains([string]$_.id) -or
        $targetRowNames.Contains([string]$_.nick) -or
        $targetItemIds.Contains([string]$_.nick) -or
        $targetServerNicks.Contains([string]$_.nick)
    }
)

$removeIds = New-Object 'System.Collections.Generic.HashSet[string]'
foreach ($row in $rowsToRemoveFromProp) {
    [void]$removeIds.Add([string]$row.id)
}

$preservedPropRows = @(
    $propRows | Where-Object {
        -not $removeIds.Contains([string]$_.id) -and
        -not $targetRowNames.Contains([string]$_.nick) -and
        -not $targetItemIds.Contains([string]$_.nick) -and
        -not $targetServerNicks.Contains([string]$_.nick)
    }
)

$preservedEquipRows = @(
    $equipRows | Where-Object {
        -not $removeIds.Contains([string]$_.id) -and
        -not $targetRowNames.Contains([string]$_.name) -and
        -not $targetItemIds.Contains([string]$_.name) -and
        -not $targetServerNicks.Contains([string]$_.name)
    }
)

$preservedConsumeRows = @(
    $consumeRows | Where-Object {
        -not $removeIds.Contains([string]$_.id)
    }
)

$serverPropRows = New-Object System.Collections.Generic.List[object]
$serverEquipRows = New-Object System.Collections.Generic.List[object]
$serverConsumeRows = New-Object System.Collections.Generic.List[object]

foreach ($row in $vendorRows) {
    $propId = Parse-PositiveInt -ValueText ([string]$row.server_propid) -DefaultValue 0
    if ($propId -le 0) {
        throw ("Invalid server_propid for {0}/{1}" -f $row.source_table, $row.row_name)
    }

    $bucket = Get-VendorServerBucket -ItemType ([string]$row.item_type)
    $minLevel = 1
    $maxCount = Parse-PositiveInt -ValueText ([string]$row.stack_max) -DefaultValue 1
    $price = 1
    $roundedValue = [Math]::Round([double]$row.value)
    $price = Parse-PositiveInt -ValueText ([string]$roundedValue) -DefaultValue 1
    $stackable = [string]$row.stackable
    $isSplit = if ($stackable -match 'True|true|1') { 1 } else { 0 }
    $serverNick = Get-VendorServerNick -SourceTable ([string]$row.source_table) -RowName ([string]$row.row_name)
    $color = Get-ServerColor ([string]$row.rarity)

    if ($bucket -eq 'equip') {
        $kind = Get-ServerEquipKind -ItemType ([string]$row.item_type) -Slot ([string]$row.slot)
        $part = Get-ServerEquipPart -ItemType ([string]$row.item_type) -Slot ([string]$row.slot)
        $rAtk = $price
        $rDef = $price

        $serverPropRows.Add([pscustomobject][ordered]@{
            id             = $propId
            type           = 1
            kind           = $kind
            color          = $color
            minlevel       = $minLevel
            maxcount       = 1
            price          = $price
            maxuse         = 0
            issplit        = 0
            isdestroy      = 1
            istransaction  = 1
            isshow         = 1
            isbind         = 0
            issell         = 1
            tab            = 1
            nick           = $serverNick
            end            = 0
        })

        $serverEquipRows.Add([pscustomobject][ordered]@{
            id         = $propId
            kind       = $kind
            level      = $minLevel
            color      = $color
            job        = 1
            part       = $part
            hp         = 0
            mp         = 0
            p_atk      = 0
            m_atk      = 0
            p_defend   = 0
            m_defend   = 0
            dodge      = 0
            crit       = 0
            r_atk      = $rAtk
            r_defend   = $rDef
            r_dodge    = 0
            r_crit     = 0
            r_hp       = 0
            name       = $serverNick
            needprop   = '0;0-0;0-0'
            end        = 0
        })
    } else {
        $consumeKind = 1
        $potionValue = $price

        $serverPropRows.Add([pscustomobject][ordered]@{
            id             = $propId
            type           = 2
            kind           = $consumeKind
            color          = $color
            minlevel       = $minLevel
            maxcount       = $maxCount
            price          = $price
            maxuse         = 0
            issplit        = $isSplit
            isdestroy      = 1
            istransaction  = 1
            isshow         = 0
            isbind         = 0
            issell         = 1
            tab            = 1
            nick           = $serverNick
            end            = 0
        })

        $serverConsumeRows.Add([pscustomobject][ordered]@{
            id            = $propId
            kind          = $consumeKind
            potion_value  = $potionValue
            potion_cdtime = 1
            gem_class     = 1
            gem_value     = 0
            end           = 0
        })
    }
}

$backupStamp = Get-Date -Format 'yyyyMMdd-HHmmss'
foreach ($path in @($propCsvPath, $equipCsvPath, $consumeCsvPath)) {
    Copy-Item $path "$path.$backupStamp.bak" -Force
}

$mergedPropRows = @($preservedPropRows + @($serverPropRows | Sort-Object id))
$mergedEquipRows = @($preservedEquipRows + @($serverEquipRows | Sort-Object id))
$mergedConsumeRows = @($preservedConsumeRows + @($serverConsumeRows | Sort-Object id))

$mergedPropRows = @(Ensure-UniquePropNick -Rows $mergedPropRows)

Write-PlainCsv -Path $propCsvPath -Headers @('id','type','kind','color','minlevel','maxcount','price','maxuse','issplit','isdestroy','istransaction','isshow','isbind','issell','tab','nick','end') -Rows $mergedPropRows
Write-PlainCsv -Path $equipCsvPath -Headers @('id','kind','level','color','job','part','hp','mp','p_atk','m_atk','p_defend','m_defend','dodge','crit','r_atk','r_defend','r_dodge','r_crit','r_hp','name','needprop','end') -Rows $mergedEquipRows
Write-PlainCsv -Path $consumeCsvPath -Headers @('id','kind','potion_value','potion_cdtime','gem_class','gem_value','end') -Rows $mergedConsumeRows

Write-Host ("Server sync complete. Wrote prop={0} equip={1} consume={2}" -f $serverPropRows.Count, $serverEquipRows.Count, $serverConsumeRows.Count)
Write-Host ("Backups created with suffix .{0}.bak" -f $backupStamp)
