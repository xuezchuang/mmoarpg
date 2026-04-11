[CmdletBinding()]
param(
    [string]$DocDir = '',
    [string]$OutputPath = '',
    [switch]$RebuildIds
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$scriptRoot = if ($PSScriptRoot) { $PSScriptRoot } else { Split-Path -Parent $MyInvocation.MyCommand.Path }
. (Join-Path $scriptRoot 'VendorPropIdRules.ps1')

if ([string]::IsNullOrWhiteSpace($DocDir)) {
    $DocDir = Join-Path $scriptRoot '..\doc'
}
if ([string]::IsNullOrWhiteSpace($OutputPath)) {
    $OutputPath = Join-Path $scriptRoot '..\doc\VendorPropExport.csv'
}

function Get-InventoryCategory {
    param([string]$ItemType)

    switch (Normalize-VendorItemType $ItemType) {
        'Weapon' { return 'Weapon' }
        'Shield' { return 'Weapon' }
        'Bow' { return 'Range' }
        'Arrow' { return 'Range' }
        'Armor' { return 'Armor' }
        'Accessories' { return 'Accessories' }
        'Food' { return 'Consumable' }
        'Potion' { return 'Consumable' }
        'Crafting_Ingredient' { return 'Resources' }
        'Currency' { return 'Resources' }
        'Other' { return 'Resources' }
        'Mining_Tool' { return 'Resources' }
        'Logging_Tool' { return 'Resources' }
        'Fishing_Tool' { return 'Resources' }
        'Horse_Saddle' { return 'Mount' }
        'Horse_Armor' { return 'Mount' }
        'Horse_Reins' { return 'Mount' }
        'Horse_Wings' { return 'Mount' }
        'Horse_Horn' { return 'Mount' }
        'Glider' { return 'Mount' }
        'Quest_Item' { return 'Quest' }
        default { return 'None' }
    }
}

function Get-PreservedServerPropIds {
    param(
        [string]$ExistingOutputPath,
        [switch]$UseExistingIds
    )

    $result = @{}
    if (-not $UseExistingIds -or -not (Test-Path $ExistingOutputPath)) {
        return $result
    }

    foreach ($row in (Import-Csv $ExistingOutputPath)) {
        $sourceTable = [string]$row.source_table
        $rowName = [string]$row.row_name
        $serverPropIdText = [string]$row.server_propid
        if ([string]::IsNullOrWhiteSpace($sourceTable) -or [string]::IsNullOrWhiteSpace($rowName)) {
            continue
        }

        $serverPropId = 0
        if (-not [int]::TryParse($serverPropIdText, [ref]$serverPropId)) {
            continue
        }
        if ($serverPropId -lt 900000000 -or $serverPropId -gt 999999999) {
            continue
        }

        $result[(Get-VendorUniqueItemKey -SourceTable $sourceTable -RowName $rowName)] = $serverPropId
    }

    return $result
}

$docRoot = (Resolve-Path $DocDir).Path
$preservedServerPropIds = Get-PreservedServerPropIds -ExistingOutputPath $OutputPath -UseExistingIds:(-not $RebuildIds)

$rows = New-Object System.Collections.Generic.List[object]
$jsonFiles = Get-ChildItem $docRoot -Filter 'DT_*.json' | Sort-Object Name

foreach ($file in $jsonFiles) {
    $tableRows = Get-Content $file.FullName -Raw | ConvertFrom-Json
    if (-not ($tableRows -is [System.Array]) -or $tableRows.Count -eq 0) {
        continue
    }

    $first = $tableRows[0]
    $props = $first.PSObject.Properties.Name
    $isItemLike = ($props -contains 'Type') -and ($props -contains 'Description') -and ($props -contains 'Stats')
    if (-not $isItemLike) {
        continue
    }

    foreach ($item in ($tableRows | Sort-Object Name)) {
        $rowName = [string]$item.Name
        if ([string]::IsNullOrWhiteSpace($rowName)) {
            continue
        }

        $itemType = [string]$item.Type
        $stackable = $null
        if ($item.Stacks.PSObject.Properties.Name -contains 'Stackable') {
            $stackable = [string]$item.Stacks.Stackable
        } elseif ($item.Stacks.PSObject.Properties.Name -contains 'Stackable?') {
            $stackable = [string]$item.Stacks.'Stackable?'
        }

        $rows.Add([pscustomobject][ordered]@{
            source_table       = $file.BaseName
            row_name           = $rowName
            item_id            = [string]$item.Description.ID
            sample_name        = [string]$item.Description.Name
            item_type          = $itemType
            inventory_category = Get-InventoryCategory $itemType
            server_bucket      = Get-VendorServerBucket $itemType
            rarity             = [string]$item.Rarity
            slot               = [string]$item.Slot
            stackable          = $stackable
            stack_max          = [string]$item.Stacks.Quantity
            value              = [string]$item.Stats.Value
            current_index      = [string]$item.Index
            server_propid      = ''
        })
    }
}

$resolvedPropIdByKey = @{}
foreach ($tableGroup in ($rows | Group-Object source_table | Sort-Object Name)) {
    $tableCode = Get-VendorTableCode -SourceTable $tableGroup.Name
    $usedSequences = New-Object 'System.Collections.Generic.HashSet[int]'
    $maxSequence = 0

    foreach ($row in ($tableGroup.Group | Sort-Object row_name)) {
        $key = Get-VendorUniqueItemKey -SourceTable ([string]$row.source_table) -RowName ([string]$row.row_name)
        if (-not $preservedServerPropIds.ContainsKey($key)) {
            continue
        }

        $existingId = [int]$preservedServerPropIds[$key]
        $expectedBucket = Get-VendorBucketCode -ItemType ([string]$row.item_type)
        $actualBucket = [int](($existingId / 1000000) % 100)
        $actualTable = [int](($existingId / 10000) % 100)
        $actualSequence = [int]($existingId % 10000)

        if ($actualBucket -ne $expectedBucket -or $actualTable -ne $tableCode -or $actualSequence -le 0) {
            continue
        }
        if (-not $usedSequences.Add($actualSequence)) {
            continue
        }

        $resolvedPropIdByKey[$key] = $existingId
        if ($actualSequence -gt $maxSequence) {
            $maxSequence = $actualSequence
        }
    }

    foreach ($row in ($tableGroup.Group | Sort-Object row_name)) {
        $key = Get-VendorUniqueItemKey -SourceTable ([string]$row.source_table) -RowName ([string]$row.row_name)
        if ($resolvedPropIdByKey.ContainsKey($key)) {
            continue
        }

        $nextSequence = if ($RebuildIds) {
            ($usedSequences.Count + 1)
        } else {
            ($maxSequence + 1)
        }

        while ($usedSequences.Contains($nextSequence)) {
            $nextSequence++
        }

        $bucketCode = Get-VendorBucketCode -ItemType ([string]$row.item_type)
        $resolvedPropIdByKey[$key] = New-VendorGeneratedPropId -BucketCode $bucketCode -TableCode $tableCode -Sequence $nextSequence
        [void]$usedSequences.Add($nextSequence)
        if ($nextSequence -gt $maxSequence) {
            $maxSequence = $nextSequence
        }
    }
}

$updatedRows = foreach ($row in ($rows | Sort-Object source_table, row_name)) {
    $copy = [ordered]@{}
    foreach ($prop in $row.PSObject.Properties.Name) {
        $copy[$prop] = $row.$prop
    }
    $copy.server_propid = [string]$resolvedPropIdByKey[(Get-VendorUniqueItemKey -SourceTable ([string]$row.source_table) -RowName ([string]$row.row_name))]
    [pscustomobject]$copy
}

$updatedRows | Export-Csv -Path $OutputPath -NoTypeInformation -Encoding UTF8

Write-Host ("Exported {0} rows -> {1}" -f $updatedRows.Count, $OutputPath)
if ($RebuildIds) {
    Write-Host 'Rebuilt all vendor prop ids from source_table + row_name.'
}
