# Server Backend

## Source of truth

When a task is really about backend code or live server logs, use the WSL repo directly instead of any stale local copy:

- Windows path: `\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg`
- WSL path: `/root/server_mmorpg`

## AI bootstrap for the server repo

- the server repo currently has `CLAUDE.md` at its root
- there is no `AGENTS.md` in the live server repo
- `.claude/settings.local.json` exists, but it does not add durable workflow rules

If a future task targets the server repo directly, read that repo's `CLAUDE.md` first, then inspect the specific server/module involved.

## Build and runtime facts

- configure once in WSL: `cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug`
- build all: `cmake --build build`
- build one target: `cmake --build build --target GameServer`
- target names are `LoginServer`, `GateServer`, `GameServer`, `DBServer`, `CenterServer`
- server CMake files use `C++20`
- built executables go to `build/bin/`
- runtime config and data live under repo `bin/`
- daily runtime logs live under `build/bin/<YYYY-MM-DD>/`

Practical split:

- `build/bin/GameServer`, `build/bin/GateServer`, etc. are compiled outputs
- `bin/config/`, `bin/*_config/`, and `bin/csv/` are data/config inputs

## Runtime operation rule

When a task changes live backend state outside the running server process, do not stop at "DB updated" or "config edited".

- direct MySQL edits for player/account/runtime-facing data should be followed by a server restart unless the task explicitly proves the data is hot-reloaded
- config changes under `bin/config/` or related runtime input folders should also be followed by a server restart unless the task explicitly proves hot-reload behavior
- after the restart, verify with both process start times and the latest server logs instead of assuming the restart succeeded

Current practical default:

- use `/root/server_mmorpg/restart.sh` to restart the whole server set
- then confirm `DBServer`, `CenterServer`, `GameServer`, `GateServer`, and `LoginServer` all have fresh start times

## Top-level layout

- `LoginServer/code`
  - account login and registration flow
- `GateServer/code`
  - client-facing routing, login/select-role/enter-world bridge
- `GameServer/code`
  - gameplay logic
- `DBServer/code`
  - persistence and DB-facing handlers
- `CenterServer/code`
  - cross-server coordination
- `engine`
  - lower-level networking implementation
- `net`
  - event-loop and TCP session framework
- `share`
  - shared protocol ids, structs, and helpers

## First files to open by task type

- login/account issues
  - `LoginServer/code/LoginServer.cpp`
  - `LoginServer/code/AppLogin.cpp`
  - `LoginServer/code/AppGate.cpp`
- gate routing, role select, enter world
  - `GateServer/code/GateServer.cpp`
  - `GateServer/code/Router.cpp`
  - `GateServer/code/AppLogin.cpp`
  - `GateServer/code/AppSelectRole.cpp`
  - `GateServer/code/AppEntryWorld.cpp`
- gameplay or ECS behavior
  - `GameServer/code/GameServer.cpp`
  - `GameServer/code/handler/`
  - `GameServer/code/service/`
  - `GameServer/code/system/`
  - `GameServer/code/entity/`
  - `GameServer/code/component/`
- DB persistence
  - `DBServer/code/DBServer.cpp`
  - `DBServer/code/DBManager*.cpp`
  - `DBServer/code/AppGame.cpp`
- cross-server team/world coordination
  - `CenterServer/code/CenterServer.cpp`
  - `CenterServer/code/AppTeam*.cpp`

## Shared protocol and data touch points

- `share/CmdDefines.h`
  - shared `CMD_*` ids across servers
- `share/UserData.h`
  - shared user and robot data structs
- `share/CommandData.h`
  - protocol payload structs
- `GameServer/code/WorldData.h`
  - gameplay world-facing data helpers used inside GameServer

## GameServer structure

`GameServer/code/` already has a clean layered split:

- `component/`
  - pure data
- `entity/`
  - `Player`, `Monster`, `Npc`
- `service/`
  - business logic
- `handler/`
  - packet decode to service dispatch
- `event/`
  - event bus and cross-system notifications
- `system/`
  - frame-driven systems

When changing gameplay logic, prefer keeping changes inside `handler -> service -> event/system` boundaries instead of reintroducing logic into network glue.

## Cross-repo workflow rule

When the client task and the server task meet at the protocol boundary:

- UE client-side proto handling starts from `UMMOARPGGameInstance` and `UMMOARPGNetSubsystem`
- server-side command ids and payload structs should be checked in `share/`
- gate/login/world-entry issues usually need both `Saved/Logs/MMOARPG.log` and `build/bin/<date>/{login,gate,center,game,db}.txt`

For future backend tasks, it is valid to edit the WSL repo directly and verify with a target-specific CMake build in that repo.
