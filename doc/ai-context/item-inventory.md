# Item And Inventory

## Core item definitions

The item system anchor is:

- `Source/MMOARPG/Data/FItemData.h`

Important enums defined there:

- `E_ItemType`
- `E_ItemRarity`
- `E_ItemSlot`
- `E_WeaponCategory`
- `E_InventoryPanels`
- `E_StatType`
- `E_StatCategory`

Important structs defined there:

- `FS_ItemDesc`
- `FF_Modifier`
- `FS_ConsumableAction`
- `FS_ItemDurability`
- `FS_ItemStats`
- `FS_ItemStacks`
- `FFS_ItemData`

## Why this file matters

If item behavior looks wrong, this header is usually the first truth source for:

- item category compatibility
- slot compatibility
- rarity naming
- stack behavior
- stat container shape

## Inventory-related client files

- `Source/MMOARPG/Inventory/UI_InventoryBase.*`
- `Source/MMOARPG/Inventory/UI_Vendor.*`
- `Source/MMOARPG/Inventory/UI_SplitStack.*`
- `Source/MMOARPG/Core/Game/MMOARPGPlayerState.*`

## Vendor propid mapping architecture

Server uses numeric propids (e.g. `800100107`). Client data tables use string `Description.ID` (e.g. `fs_hm`). The bridge is:

1. `AMMOARPGGameState::VendorItemIdToPropId` — `TMap<FString, int32>` populated in the constructor
2. `ApplyVendorPropIdMap()` called at BeginPlay — normalizes both keys and `Description.ID` (lowercase, alphanumeric only), then writes `Item.Index = propid` for each match
3. After BeginPlay, `FindItemByIndex(propid)` works for any item whose `Description.ID` was registered

**When a new vendor item type can't be bought or doesn't appear in the bag after sync**, the first check is: is its `Description.ID` in `VendorItemIdToPropId`? Source of truth for the mapping: `doc/VendorPropExport.csv` (column `item_id` → `server_propid`) cross-referenced with `server_mmorpg/bin/csv/prop.csv` (column `nick` = client ID, `id` = server propid).

The same mapping is also hardcoded as a fallback in `UI_SplitStack::ResolveBuyPropId` (`FallbackVendorIdMap`). If you add a new vendor item, add it to **both** places.

Files:
- `Source/MMOARPG/Core/Game/MMOARPGGameState.cpp` — constructor + `ApplyVendorPropIdMap`
- `Source/MMOARPG/Inventory/UI_SplitStack.cpp` — `ResolveBuyPropId` fallback map

## Inventory UI widget tree and refresh

**`UWidgetTree::GetAllWidgets` does not recurse into child `UUserWidget` trees.** It only searches the current widget's own tree. `UUI_InventoryBase` is nested multiple layers inside `UUI_CharacterMenu` via intermediate user widgets, so a naive `GetAllWidgets` on CharacterMenu will not find it.

Fix in place: `FindInventoryWidgetsRecursive` in `UI_CharacterMenu.cpp` manually recurses into each child `UUserWidget`'s `WidgetTree`. `RefreshInventoryWidgets` uses this.

**`NativeConstruct` fires only once** when the widget is first created. Subsequent Show/Hide (via `SetVisibility`) does not re-call it. `RefreshInventoryWidgets` → `RefreshCurrentCategory` is called from `OpenMenu()` every time the menu opens, which is the correct hook for re-populating the grid with current `BagItems`.

## Practical debugging reminders

- verify whether item data problems are data-table issues or code issues
- distinguish server accepted buy/sell from client UI refresh failure
- check whether bag data is written into local runtime state before blaming widgets
- when packet-level issues appear, inspect `Plugins/SimpleNetChannel` before rewriting UI logic
- `[InventorySync] Ignore unknown bag item [propid:X]` → propid X is missing from `VendorItemIdToPropId` in GameState constructor
- `[VendorUI] Block SP_ItemBuy because propid is invalid` → item ID missing from `FallbackVendorIdMap` in `UI_SplitStack::ResolveBuyPropId`
