# Project Overview

## What this repo is

- Unreal Engine `5.2`
- main module: `Source/MMOARPG`
- multiplayer action RPG client with Blueprint support
- custom plugin-heavy codebase

## Main code areas

- `Source/MMOARPG/Core/Game`
  - game mode, player controller, character classes
- `Source/MMOARPG/Core/Common`
  - `GameInstance`, net subsystem, world-level helpers
- `Source/MMOARPG/Core/Component`
  - reusable gameplay components
- `Source/MMOARPG/NetPlay`
  - BladeII-specific network gameplay code
- `Source/MMOARPG/Inventory`
  - inventory and vendor UI logic
- `Source/MMOARPG/Data`
  - gameplay data structs such as item definitions
- `Source/MMOARPG/DataTable`
  - animation and style data tables

## Important plugin dependencies

- `Plugins/MMOARPGCommon`
  - shared types and protocol-facing data
- `Plugins/SimpleNetChannel`
  - transport, channel, packet send/recv
- `Plugins/SimpleCombat`
  - combat support
- `Plugins/SimpleAdvancedAnimation`
  - animation support
- `Plugins/SimpleMySQL`
  - MySQL integration
- `Plugins/SimpleHTTP`
  - HTTP integration

## Useful entry points

- `Source/MMOARPG/Core/Common/MMOARPGGameInstance.*`
  - runtime startup, client creation, QuickTest flow
- `Source/MMOARPG/Core/Common/MMOARPGNetSubsystem.*`
  - protocol binding and packet dispatch
- `Source/MMOARPG/Core/Game/Character/Core/MMOARPGCharacterBase.h`
  - shared gameplay character base
- `Source/MMOARPG/Data/FItemData.h`
  - core item enums and structs

## External source of truth during network debugging

Do not assume the repo-local backend copy is current.

Use:

- `\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg`

Daily logs live under:

- `\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg\build\bin\<YYYY-MM-DD>\`
