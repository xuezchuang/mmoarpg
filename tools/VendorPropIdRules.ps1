[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Normalize-VendorItemType {
    param([string]$ItemType)

    if ([string]::IsNullOrWhiteSpace($ItemType)) {
        return ''
    }

    return (($ItemType -replace '\s+', '_').Trim())
}

function Get-VendorUniqueItemKey {
    param(
        [string]$SourceTable,
        [string]$RowName
    )

    return ('{0}::{1}' -f $SourceTable.Trim(), $RowName.Trim())
}

function Get-VendorServerNick {
    param(
        [string]$SourceTable,
        [string]$RowName
    )

    return ('{0}__{1}' -f $SourceTable.Trim(), $RowName.Trim())
}

function Get-VendorTableCode {
    param([string]$SourceTable)

    switch ($SourceTable) {
        'DT_Weapons' { return 1 }
        'DT_Armor' { return 2 }
        'DT_Accessory' { return 3 }
        'DT_Consumables' { return 4 }
        'DT_Cooking' { return 5 }
        'DT_CraftingIngredients' { return 6 }
        'DT_Fish_Ingredients' { return 7 }
        'DT_GatherResources' { return 8 }
        'DT_Glider' { return 9 }
        default { throw "Unsupported source table: $SourceTable" }
    }
}

function Get-VendorBucketCode {
    param([string]$ItemType)

    switch (Normalize-VendorItemType $ItemType) {
        'Weapon' { return 10 }
        'Shield' { return 10 }
        'Bow' { return 10 }
        'Arrow' { return 10 }
        'Armor' { return 10 }
        'Accessories' { return 10 }
        'Horse_Saddle' { return 10 }
        'Horse_Armor' { return 10 }
        'Horse_Reins' { return 10 }
        'Horse_Wings' { return 10 }
        'Horse_Horn' { return 10 }
        'Glider' { return 10 }
        'Food' { return 20 }
        'Potion' { return 20 }
        'Crafting_Ingredient' { return 20 }
        'Currency' { return 20 }
        'Other' { return 20 }
        'Mining_Tool' { return 20 }
        'Logging_Tool' { return 20 }
        'Fishing_Tool' { return 20 }
        'Quest_Item' { return 30 }
        default { return 30 }
    }
}

function Get-VendorServerBucket {
    param([string]$ItemType)

    switch (Get-VendorBucketCode $ItemType) {
        10 { return 'equip' }
        default { return 'consume' }
    }
}

function New-VendorGeneratedPropId {
    param(
        [int]$BucketCode,
        [int]$TableCode,
        [int]$Sequence
    )

    if ($BucketCode -lt 0 -or $BucketCode -gt 99) {
        throw "Invalid bucket code: $BucketCode"
    }
    if ($TableCode -lt 0 -or $TableCode -gt 99) {
        throw "Invalid table code: $TableCode"
    }
    if ($Sequence -lt 1 -or $Sequence -gt 9999) {
        throw "Invalid table sequence: $Sequence"
    }

    return (900000000 + ($BucketCode * 1000000) + ($TableCode * 10000) + $Sequence)
}
