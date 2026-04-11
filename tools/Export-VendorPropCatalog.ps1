[CmdletBinding()]
param(
    [string]$DocDir = '',
    [string]$OutputPath = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$scriptRoot = if ($PSScriptRoot) { $PSScriptRoot } else { Split-Path -Parent $MyInvocation.MyCommand.Path }
if ([string]::IsNullOrWhiteSpace($DocDir)) {
    $DocDir = Join-Path $scriptRoot '..\doc'
}
if ([string]::IsNullOrWhiteSpace($OutputPath)) {
    $OutputPath = Join-Path $scriptRoot '..\doc\VendorPropExport.csv'
}

function Get-InventoryCategory {
    param([string]$ItemType)

    switch ($ItemType) {
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

function Get-ServerBucket {
    param([string]$ItemType)

    switch ($ItemType) {
        'Weapon' { return 'equip' }
        'Shield' { return 'equip' }
        'Bow' { return 'equip' }
        'Arrow' { return 'equip' }
        'Armor' { return 'equip' }
        'Accessories' { return 'equip' }
        'Horse_Saddle' { return 'equip' }
        'Horse_Armor' { return 'equip' }
        'Horse_Reins' { return 'equip' }
        'Horse_Wings' { return 'equip' }
        'Horse_Horn' { return 'equip' }
        'Glider' { return 'equip' }
        'Food' { return 'consume' }
        'Potion' { return 'consume' }
        'Crafting_Ingredient' { return 'prop' }
        'Currency' { return 'prop' }
        'Other' { return 'prop' }
        'Mining_Tool' { return 'prop' }
        'Logging_Tool' { return 'prop' }
        'Fishing_Tool' { return 'prop' }
        'Quest_Item' { return 'prop' }
        default { return 'unknown' }
    }
}

$docRoot = (Resolve-Path $DocDir).Path
$existingPropIdByItemId = @{}

if (Test-Path $OutputPath) {
    foreach ($row in (Import-Csv $OutputPath)) {
        if ($row.item_id -and $row.server_propid) {
            $existingPropIdByItemId[[string]$row.item_id] = [string]$row.server_propid
        }
    }
}

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

    foreach ($item in $tableRows) {
        $itemId = [string]($item.Description.ID)
        if ([string]::IsNullOrWhiteSpace($itemId)) {
            $itemId = [string]$item.Name
        }

        $stackable = $null
        if ($item.Stacks.PSObject.Properties.Name -contains 'Stackable') {
            $stackable = [string]$item.Stacks.Stackable
        } elseif ($item.Stacks.PSObject.Properties.Name -contains 'Stackable?') {
            $stackable = [string]$item.Stacks.'Stackable?'
        }

        $stackMax = [string]$item.Stacks.Quantity
        $serverPropId = ''
        if ($existingPropIdByItemId.ContainsKey($itemId)) {
            $serverPropId = $existingPropIdByItemId[$itemId]
        }

        $rows.Add([pscustomobject][ordered]@{
            source_table       = $file.BaseName
            row_name           = [string]$item.Name
            item_id            = $itemId
            sample_name        = [string]$item.Description.Name
            item_type          = [string]$item.Type
            inventory_category = Get-InventoryCategory ([string]$item.Type)
            server_bucket      = Get-ServerBucket ([string]$item.Type)
            rarity             = [string]$item.Rarity
            slot               = [string]$item.Slot
            stackable          = $stackable
            stack_max          = $stackMax
            value              = [string]$item.Stats.Value
            current_index      = [string]$item.Index
            server_propid      = $serverPropId
        })
    }
}

$rows |
    Sort-Object source_table, item_id, row_name |
    Export-Csv -Path $OutputPath -NoTypeInformation -Encoding UTF8

Write-Host ("Exported {0} rows -> {1}" -f $rows.Count, $OutputPath)
