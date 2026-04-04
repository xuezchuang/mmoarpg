# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

An Unreal Engine 5.2 multiplayer online action RPG (MMOARPG) written in C++ with Blueprint support. The project uses a client-server architecture with dedicated server support and includes custom plugins for networking, combat, and animation.

## Build Commands

### Claude Code 编译命令
每次修改完代码后，使用以下命令编译：
```bash
dotnet "D:/ue/UE_5.2/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll" MMOARPGEditor Win64 Development -Project="D:/ueDemo/MMOARPG/MMOARPG.uproject"
```

### 手动编译
- **Open project**: Double-click `MMOARPG.uproject` or open `MMOARPG.sln` in Visual Studio
- **Build solution**: Open `MMOARPG.sln` in Visual Studio and build the `Solution` configuration
- **Editor run**: Use Unreal Editor `Play` or `Launch` buttons for local testing
- **Server build**: Build the `MMOARPGServer` target for dedicated server

## Architecture

### Character Class Hierarchy

```
ACharacter (UE)
└── AMMOARPGCharacterBase (Core/Game/Character/Core/)
    ├── AMMOARPGCharacter     - Main playable character
    ├── AMMOARPGMonster        - Monster enemies
    ├── AMMOARPGNPC            - Non-player characters
    ├── AMMOARPGPlayerCharacter - Player-specific character
    └── BladeIICharacter      - Alternative combat character
```

### Key Source Directories

| Path | Purpose |
|------|---------|
| `Source/MMOARPG/Core/Game/` | Game mode, player controller, character classes |
| `Source/MMOARPG/Core/Component/` | Reusable components (FlyComponent, InteractionComponent, InventoryComponent) |
| `Source/MMOARPG/Core/Common/` | GameInstance, NetSubsystem, WorldSubsystems |
| `Source/MMOARPG/NetPlay/` | BladeII networking system (GameMode, Character, Player) |
| `Source/MMOARPG/Inventory/` | Inventory system UI and logic |
| `Source/MMOARPG/Data/` | Item data structures (FItemData.h) |
| `Source/MMOARPG/DataTable/` | Animation and style data tables |

### Custom Plugins (in `Plugins/`)

- **MMOARPGCommon** - Shared types, protocols, and data structures (FMMOARPGUserData, S_ROLE_BASE, etc.)
- **SimpleNetChannel** - Networking channel system (FSimpleNetManage, FSimpleChannel)
- **SimpleCombat** - Combat system implementation
- **SimpleAdvancedAnimation** - Advanced animation system
- **SimpleMySQL** - Database connectivity
- **SimpleHTTP** - HTTP request handling

### Networking Architecture

The game uses a custom networking system with `FSimpleNetManage`:
- `UMMORPGNetSubsystem` - GameInstanceSubsystem managing network connections and protocol routing
- `ABladeIINetGameMode` - Net play game mode handling player connections
- Protocol-based communication via `FSimpleChannel`
- Gate server and login server connection support

### Runtime Logging And Protocol Notes

- Main client runtime log: `Saved/Logs/MMOARPG.log`
- `Proto 60000` is heartbeat traffic from `SimpleNetChannel`
- `Proto 900` is `SP_EnterWorld`
- Socket connection success does not imply business login success; handshake and business response must be checked separately

### QuickTest Notes

- QuickTest bootstrap is centered in `Source/MMOARPG/Core/Common/MMOARPGGameInstance.*`
- Default QuickTest account in code is `xuezc_1`
- Default QuickTest password in code is `123456`
- QuickTest login failure should actively close the connection
- `SP_EnterWorld` should only be sent after successful `SP_CharacterResponse` and post-travel world readiness, not immediately on connect

### Item System

Item data structures defined in `Data/FItemData.h`:
- `E_ItemType` - Item categories (Weapon, Armor, Potion, etc.)
- `E_ItemRarity` - Rarity levels
- `E_ItemSlot` - Equipment slots
- `FFS_ItemData` - Main item data table row structure
- `FS_ItemStats`, `FS_ItemStacks` - Item statistics and stacking

### UI System

UI classes in `Inventory/` and `UI/` directories:
- `UI_InventoryBase` - Base inventory widget
- `UI_CraftingWindow` - Crafting interface
- `UI_Vendor` - Vendor/shop interface
- `UI_MainGUI` - Main game HUD
- `UI_WindowSwitcher` - Manages window visibility

### Game State Management

- `AMMOARPGGameMode` - Base game mode
- `AMMOARPGPlayerController` - Player input and control
- `AMMOARPGGameInstance` - Persistent game instance with `FSimpleNetManage` client
- `MonsterWorldSubsystem` - Manages monster spawns and AI
- `PlayerWorldSubsystem` - Manages player-specific world data

## Important Files

- `MMOARPGType.h` (in MMOARPGCommon plugin) - Core game types, character action states, protocol numbers
- `FItemData.h` - Complete item system enums and structures
- `MMOARPGCharacterBase.h` - Base character with replicated action state
- `MMOARPGGameInstance.h` - Game instance with network client management
