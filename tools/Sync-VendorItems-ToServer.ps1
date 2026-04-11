[CmdletBinding()]
param(
    [string]$VendorExportPath = '',
    [string]$ServerCsvDir = '\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg\bin\csv'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$scriptRoot = if ($PSScriptRoot) { $PSScriptRoot } else { Split-Path -Parent $MyInvocation.MyCommand.Path }
if ([string]::IsNullOrWhiteSpace($VendorExportPath)) {
    $VendorExportPath = Join-Path $scriptRoot '..\doc\VendorPropExport.csv'
}

function Get-ServerBucketForSync {
    param([string]$ItemType)

    switch ($ItemType) {
        'Weapon' { return 'equip' }
        'Shield' { return 'equip' }
        'Bow' { return 'equip' }
        'Arrow' { return 'equip' }
        'Armor' { return 'equip' }
        'Accessories' { return 'equip' }
        default { return 'consume' }
    }
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

    if ($ItemType -in @('Weapon', 'Shield', 'Bow', 'Arrow')) { return 1 }

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

    if ($ItemType -in @('Weapon', 'Shield', 'Bow', 'Arrow')) { return 0 }

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

function Parse-StatInt {
    param($Row, [string]$Name)
    if ($null -eq $Row) { return 0 }
    if ($Row.PSObject.Properties.Name -notcontains $Name) { return 0 }
    $n = 0.0
    if ([double]::TryParse([string]$Row.$Name, [ref]$n)) {
        return [int][Math]::Round($n)
    }
    return 0
}

function Get-MaxExistingId {
    param(
        [object[]]$Rows,
        [int]$Fallback
    )

    $maxId = $Fallback
    foreach ($row in $Rows) {
        $id = 0
        if ([int]::TryParse([string]$row.id, [ref]$id) -and $id -gt $maxId) {
            $maxId = $id
        }
    }
    return $maxId
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

$vendorRows = Import-Csv $VendorExportPath
$duplicateGroups = $vendorRows | Group-Object item_id | Where-Object { $_.Count -gt 1 }

foreach ($group in $duplicateGroups) {
    Write-Warning ("Duplicate item_id collapsed for server sync: {0} ({1} rows)" -f $group.Name, $group.Count)
}

$uniqueByItemId = [ordered]@{}
foreach ($row in ($vendorRows | Sort-Object { [string]::IsNullOrWhiteSpace($_.server_propid) }, source_table, row_name)) {
    if (-not $uniqueByItemId.Contains($row.item_id)) {
        $uniqueByItemId[$row.item_id] = $row
    }
}

$propCsvPath = Join-Path $ServerCsvDir 'prop.csv'
$equipCsvPath = Join-Path $ServerCsvDir 'equip.csv'
$consumeCsvPath = Join-Path $ServerCsvDir 'consume.csv'

$propRows = @(Import-Csv $propCsvPath)
$equipRows = @(Import-Csv $equipCsvPath)
$consumeRows = @(Import-Csv $consumeCsvPath)

$existingPropById = @{}
foreach ($row in $propRows) {
    $existingPropById[[string]$row.id] = $true
}

$existingEquipPropRows = @($propRows | Where-Object { [string]$_.type -eq '1' })
$existingConsumePropRows = @($propRows | Where-Object { [string]$_.type -eq '2' })

$nextEquipId = [Math]::Max((Get-MaxExistingId $existingEquipPropRows 100900000), (Get-MaxExistingId $equipRows 100900000))
if ($nextEquipId -lt 100900000) { $nextEquipId = 100900000 }
$nextConsumeId = [Math]::Max((Get-MaxExistingId $existingConsumePropRows 210500100), (Get-MaxExistingId $consumeRows 210500100))
if ($nextConsumeId -lt 210500100) { $nextConsumeId = 210500100 }

$resolvedPropIdByItemId = @{}
foreach ($pair in $uniqueByItemId.GetEnumerator()) {
    $row = $pair.Value
    $existingId = [string]$row.server_propid
    if (-not [string]::IsNullOrWhiteSpace($existingId)) {
        $resolvedPropIdByItemId[$pair.Key] = $existingId
        continue
    }

    $bucket = Get-ServerBucketForSync ([string]$row.item_type)
    if ($bucket -eq 'equip') {
        $nextEquipId++
        $resolvedPropIdByItemId[$pair.Key] = [string]$nextEquipId
    } else {
        $nextConsumeId++
        $resolvedPropIdByItemId[$pair.Key] = [string]$nextConsumeId
    }
}

$serverPropRows = New-Object System.Collections.Generic.List[object]
$serverEquipRows = New-Object System.Collections.Generic.List[object]
$serverConsumeRows = New-Object System.Collections.Generic.List[object]

foreach ($pair in $uniqueByItemId.GetEnumerator()) {
    $row = $pair.Value
    $propId = $resolvedPropIdByItemId[$pair.Key]
    if ($existingPropById.ContainsKey($propId)) {
        continue
    }

    $bucket = Get-ServerBucketForSync ([string]$row.item_type)
    $minLevel = 1
    $tmpLevel = 0
    if ([int]::TryParse([string]$row.current_index, [ref]$tmpLevel) -and $tmpLevel -gt 0) {
        $minLevel = $tmpLevel
    }

    $maxCount = 1
    $tmpCount = 0
    if ([int]::TryParse([string]$row.stack_max, [ref]$tmpCount) -and $tmpCount -gt 0) {
        $maxCount = $tmpCount
    }

    $price = 1
    $tmpPrice = 0
    if ([int]::TryParse([string]([math]::Round([double]$row.value)), [ref]$tmpPrice) -and $tmpPrice -gt 0) {
        $price = $tmpPrice
    }

    $stackable = [string]$row.stackable
    $isSplit = if ($stackable -match 'True|true|1') { 1 } else { 0 }
    $nick = ([string]$row.item_id) -replace ',', '_'
    $color = Get-ServerColor ([string]$row.rarity)

    if ($bucket -eq 'equip') {
        $kind = Get-ServerEquipKind -ItemType ([string]$row.item_type) -Slot ([string]$row.slot)
        $part = Get-ServerEquipPart -ItemType ([string]$row.item_type) -Slot ([string]$row.slot)
        $rAtk = $price
        $rDef = $price
        $rHp = 0
        $rCrit = 0

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
            nick           = $nick
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
            r_crit     = $rCrit
            r_hp       = $rHp
            name       = $nick
            needprop   = '0;0-0;0-0'
            end        = 0
        })
    } else {
        $consumeKind = 1
        $potionValue = 1
        if ($price -gt 0) {
            $potionValue = $price
        }

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
            nick           = $nick
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

$mergedPropRows = @($propRows + $serverPropRows)
$mergedEquipRows = @($equipRows + $serverEquipRows)
$mergedConsumeRows = @($consumeRows + $serverConsumeRows)

Write-PlainCsv -Path $propCsvPath -Headers @('id','type','kind','color','minlevel','maxcount','price','maxuse','issplit','isdestroy','istransaction','isshow','isbind','issell','tab','nick','end') -Rows $mergedPropRows
Write-PlainCsv -Path $equipCsvPath -Headers @('id','kind','level','color','job','part','hp','mp','p_atk','m_atk','p_defend','m_defend','dodge','crit','r_atk','r_defend','r_dodge','r_crit','r_hp','name','needprop','end') -Rows $mergedEquipRows
Write-PlainCsv -Path $consumeCsvPath -Headers @('id','kind','potion_value','potion_cdtime','gem_class','gem_value','end') -Rows $mergedConsumeRows

$updatedVendorRows = foreach ($row in $vendorRows) {
    $copy = [ordered]@{}
    foreach ($prop in $row.PSObject.Properties.Name) {
        $copy[$prop] = $row.$prop
    }
    $copy['server_propid'] = $resolvedPropIdByItemId[[string]$row.item_id]
    [pscustomobject]$copy
}

$updatedVendorRows | Export-Csv -Path $VendorExportPath -NoTypeInformation -Encoding UTF8

Write-Host ("Server sync complete. Added prop={0} equip={1} consume={2}" -f $serverPropRows.Count, $serverEquipRows.Count, $serverConsumeRows.Count)
Write-Host ("Backups created with suffix .{0}.bak" -f $backupStamp)
