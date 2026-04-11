# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## AI Context Pack

If `doc/ai-context/README.md` exists, read it early in the task after this file. Then read only the topic files relevant to the user's request.

Use `doc/ai-context/` as a maintained memory layer for durable repository knowledge:

- architecture and module boundaries
- protocol and login flow constraints
- debugging playbooks
- inventory, UI, or server integration notes

When the user asks to "summarize and save", update the matching file in `doc/ai-context/` with stable findings from the current session. Do not store temporary speculation or raw log spam there.

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
- Active server logs and server-side code should be checked under `\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg`
- Daily server logs live under `\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg\build\bin\<YYYY-MM-DD>\`
- Common server log files are `login.txt`, `gate.txt`, `center.txt`, `game.txt`, and `db.txt`
- The date directory changes and should be discovered dynamically when debugging
- If the repository-local `server_mmorpg` folder is deleted, continue using the WSL path above as the source of truth
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

---

## Server Development

> 完整服务器开发指南（架构、协议、构建）见 `doc/server/SERVER_GUIDE.md`

服务器代码路径（工具访问）：`//wsl.localhost/Ubuntu-22.04/root/server_mmorpg`

### 服务器构建（在 WSL 中运行）

```bash
# 编译全部
wsl -d Ubuntu-22.04 -e bash -c "cd /root/server_mmorpg && cmake --build build"

# 编译指定服务器（例如 GameServer）
wsl -d Ubuntu-22.04 -e bash -c "cd /root/server_mmorpg && cmake --build build --target GameServer"
```

### 服务器目录结构速查

| 服务器 | 路径 | 职责 |
|--------|------|------|
| LoginServer | `LoginServer/code/` | 账号登录/注册 |
| GateServer | `GateServer/code/` | 客户端路由（纯路由，Router.cpp统一转发） |
| GameServer | `GameServer/code/` | 游戏逻辑（ECS架构：component/entity/service/handler/event/system） |
| DBServer | `DBServer/code/` | 数据库持久化 |
| CenterServer | `CenterServer/code/` | 跨服协调 |
| 共享定义 | `share/CmdDefines.h` | 全部 CMD_* 协议号 |
| 共享数据 | `share/UserData.h` | S_USER_BASE（玩家）、S_ROBOT_BASE（怪物/NPC） |

### GameServer ECS 层次（重构已全部完成 Phase 1-8）

```
component/  → 纯数据（BagComponent, StatsComponent, PositionComponent …）
entity/     → Player / Monster / Npc（包装 S_USER_BASE/S_ROBOT_BASE）
service/    → 纯业务逻辑（InventoryService, SkillService, CombatService …）
handler/    → 薄协议层（解包→调Service→回包）
event/      → EventBus（OnMonsterKilled, OnItemAdded …）
system/     → 帧驱动（BuffSystem, CooldownSystem, AiSystem …）
```

### 服务器日志

```
//wsl.localhost/Ubuntu-22.04/root/server_mmorpg/build/bin/<YYYY-MM-DD>/
```
日期目录动态变化，每次调试需查找最新日期目录。日志文件：`login.txt`, `gate.txt`, `game.txt`, `db.txt`, `center.txt`
