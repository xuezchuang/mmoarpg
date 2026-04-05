# AGENTS.md

This file defines the working rules for Codex in this repository.

## Initialization

At the start of every new task or new conversation in this repository, Codex should read this file first before making assumptions, proposing edits, or changing code.

If [`CLAUDE.md`](./CLAUDE.md) also exists, treat it as supplemental project context. When `AGENTS.md` and `CLAUDE.md` overlap, prefer `AGENTS.md` for Codex-specific behavior and use `CLAUDE.md` for project background.

## Project Overview

This repository is an Unreal Engine 5.2 multiplayer online action RPG (MMOARPG) written in C++ with Blueprint support.

The project uses a client-server architecture with dedicated server support and includes custom plugins for networking, combat, animation, HTTP, MySQL, and shared gameplay data.

## Core Directories

| Path | Purpose |
|------|---------|
| `Source/MMOARPG/` | Main game module |
| `Source/MMOARPG/Core/Game/` | Game mode, player controller, character classes |
| `Source/MMOARPG/Core/Component/` | Reusable gameplay components |
| `Source/MMOARPG/Core/Common/` | GameInstance, NetSubsystem, world subsystems |
| `Source/MMOARPG/NetPlay/` | BladeII networking system |
| `Source/MMOARPG/Inventory/` | Inventory logic and UI |
| `Source/MMOARPG/Data/` | Gameplay/item data structures |
| `Source/MMOARPG/DataTable/` | Animation and style data tables |
| `Plugins/` | Custom shared systems and engine extensions |

## Important Systems

### Character Hierarchy

`AMMOARPGCharacterBase` is the central gameplay character base type.

Derived classes include:
- `AMMOARPGCharacter`
- `AMMOARPGMonster`
- `AMMOARPGNPC`
- `AMMOARPGPlayerCharacter`
- `BladeIICharacter`

### Networking

The project uses a custom networking stack built around `FSimpleNetManage` and `FSimpleChannel`.

Key integration points:
- `UMMORPGNetSubsystem`
- `ABladeIINetGameMode`
- Gate server and login server communication

### Item/Data

The item system is centered around `Source/MMOARPG/Data/FItemData.h`.

Key types include:
- `E_ItemType`
- `E_ItemRarity`
- `E_ItemSlot`
- `FFS_ItemData`
- `FS_ItemStats`
- `FS_ItemStacks`

### Plugins

Important plugins include:
- `MMOARPGCommon`
- `SimpleNetChannel`
- `SimpleCombat`
- `SimpleAdvancedAnimation`
- `SimpleMySQL`
- `SimpleHTTP`

## Build And Verification

After modifying C++ code, prefer verifying with UnrealBuildTool using this command:

```powershell
dotnet "D:/ue/UE_5.2/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll" MMOARPGEditor Win64 Development -Project="D:/ueDemo/MMOARPG/MMOARPG.uproject"
```

When the task affects dedicated server behavior, also consider whether `MMOARPGServer` should be built or validated.

If a full build is too expensive for the current task, state clearly what was not verified.

## Logging And Debugging

### Primary Client Log

For client-side runtime issues, check:

- `Saved/Logs/MMOARPG.log`

### Primary Server Logs And Server Code

Do not rely on the repository-local `server_mmorpg` copy as the long-term source of truth.

The active server logs and server-side code to inspect during debugging are under:

- `\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg`

In particular:

- Active daily server logs live under `\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg\build\bin\<YYYY-MM-DD>\`
- Typical files are `login.txt`, `gate.txt`, `center.txt`, `game.txt`, and `db.txt`
- The date directory changes over time and should be discovered dynamically instead of hardcoding a single date

If the repository copy of `server_mmorpg` is missing or deleted, Codex should continue debugging by reading code and logs from the WSL path above.

If UE keeps the log file open, prefer closing PIE or the editor before truncating or replacing the log.

### Quick Network Triage Order

When debugging login, gate, or scene-entry issues, inspect the chain in this order:

1. `Connect Success`
2. `Recv SP_RCODE`
3. `Send SP_SECURITY`
4. `Recv SP_SECURITY`
5. `Send/Recv SP_LoginResponses`
6. `Send/Recv SP_CharacterLogin`
7. `Send/Recv SP_CharacterSelect`
8. `Recv SP_CharacterResponse`
9. `SP_EnterWorld`

For cross-service issues, correlate the client log with:

1. `login.txt`
2. `gate.txt`
3. `center.txt`
4. `game.txt`
5. `db.txt`

If the user asks why a login succeeded or failed, do not stop at socket connection. Distinguish:

- socket connection success
- security handshake success
- business login success

### Protocol Notes

- `CMD/Proto 60000` is heartbeat traffic from `SimpleNetChannel`, not gameplay login logic.
- `CMD/Proto 900` is `SP_EnterWorld`.
- If `SP_LoginResponses` returns non-zero `childcmd`, treat it as business login failure even if the socket and handshake succeeded.

### Current QuickTest Rules

- QuickTest is implemented in `Source/MMOARPG/Core/Common/MMOARPGGameInstance.*`.
- QuickTest should start from the current PIE gameplay map and must not redirect to `Login` just to bootstrap.
- QuickTest should not send gameplay protocol `SP_EnterWorld` merely because the socket connected.
- `SP_EnterWorld` should only be sent after a successful `SP_CharacterResponse`, after map travel completes, and only once per travel.
- On QuickTest login failure, the client should stop the QuickTest flow and actively close the connection so heartbeat does not continue indefinitely.

### Known Project Defaults

- Current QuickTest default account is `xuezc_1`.
- Current QuickTest default password in code is `123456`.
- These defaults live in `Source/MMOARPG/Core/Common/MMOARPGGameInstance.h` and may also be overridden in Blueprint defaults.

## Working Rules For Codex

1. Read this file before starting substantial work in this repository.
2. Inspect the relevant code before editing. Do not guess architecture from filenames alone.
3. Prefer minimal, local changes that match the existing Unreal coding style.
4. Preserve user changes already present in the worktree. Do not revert unrelated modifications.
5. When touching gameplay, networking, inventory, or UI code, respect the module boundaries listed above.
6. If a task changes code, try to compile or otherwise validate the affected area before finishing.
7. If validation cannot be completed, say exactly why.
8. When a request is ambiguous, resolve it from repository context first instead of immediately asking.
9. When referencing repository files to the user, use clickable VS Code workspace file links whose targets are absolute paths under this project root, formatted like `/d:/ueDemo/MMOARPG/...`.
10. Prefer labels like `UI_LoginMain.cpp (line 227)` and include the line number in the target when possible, for example `/d:/ueDemo/MMOARPG/Source/MMOARPG/UI/Login/UI_LoginMain.cpp#L227`.
11. Do not use webpage-style or connector-style file references for repository files in this project.

## Files Worth Checking Early

- `CLAUDE.md`
- `MMOARPG.uproject`
- `Saved/Logs/MMOARPG.log`
- `Source/MMOARPG/Core/Common/MMOARPGGameInstance.h`
- `Source/MMOARPG/Core/Common/MMOARPGGameInstance.cpp`
- `Source/MMOARPG/Core/Common/MMOARPGNetSubsystem.cpp`
- `Source/MMOARPG/Core/Game/Character/Core/MMOARPGCharacterBase.h`
- `Source/MMOARPG/Data/FItemData.h`
- `Plugins/MMOARPGCommon/`

## Expected Usage

For a new conversation, the user can simply say:

`先读取 AGENTS.md 做初始化`

That is sufficient to restore the repository-specific working rules for Codex.
