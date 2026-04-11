# Codex Handoff

Last updated: 2026-04-11

## How to use this file in a new chat

Ask the next AI to read these files first:

1. `AGENTS.md`
2. `doc/CODEX_HANDOFF.md`
3. `Saved/Logs/MMOARPG.log` if the issue is runtime-only

Recommended prompt:

`先读 AGENTS.md 和 doc/CODEX_HANDOFF.md 做初始化，然后继续排查商店购买和背包同步问题`

## Project shape

- Unreal Engine 5.2 multiplayer RPG client in this repo.
- Active server code and logs are not the repo-local copy. Use:
  - `\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg`
- Active daily server logs are under:
  - `\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg\build\bin\<YYYY-MM-DD>\`
- Usual logs:
  - `login.txt`
  - `gate.txt`
  - `center.txt`
  - `game.txt`
  - `db.txt`

## Important client modules

- `Source/MMOARPG/Core/Common/MMOARPGGameInstance.*`
  - QuickTest flow and login bootstrap
- `Source/MMOARPG/Core/Common/MMOARPGNetSubsystem.*`
  - protocol registration and dispatch
- `Source/MMOARPG/Inventory/UI_Vendor.*`
  - vendor UI
- `Source/MMOARPG/Inventory/UI_SplitStack.*`
  - buy/sell popup and current `SP_ItemBuy` handling
- `Source/MMOARPG/Inventory/UI_InventoryBase.*`
  - inventory rendering
- `Source/MMOARPG/Core/Game/MMOARPGPlayerState.*`
  - current local bag cache used by inventory UI
- `Plugins/SimpleNetChannel/...`
  - custom network layer, packet framing, handshake, send/recv

## Important server modules

- `GameServer/code/service/InventoryService.cpp`
  - buy/sell, update bag, update gold
- `GameServer/code/handler/BagHandler.cpp`
  - bag protocol handling
- `GameServer/code/WorldBC.cpp`
  - broadcasts `CMD_800`, `CMD_760`, etc.
- `GateServer/code/Router.cpp`
  - forwards `CMD_800`, `CMD_760`, `CMD_880`, etc. to client

## Purchase flow

Current expected vendor buy chain:

1. Client sends `SP_ItemBuy (880)`
2. Gate forwards to Game
3. Game `InventoryService::buyItem()` creates or stacks item
4. Game sends:
   - `CMD_800` bag update
   - `CMD_760` gold update
   - `CMD_880` buy result
5. Gate forwards those packets to client
6. Client should update local bag/UI

## What was already fixed before

Earlier sessions already addressed these issues:

- low starting gold causing buy failure
- some item config/server-side create failures such as `3006`
- vendor item id / prop id mapping work on client side
- basic logging around buy flow

Those were not the latest blocker.

## Latest root cause found on 2026-04-11

The latest blocker was not MySQL or GameServer business logic.

The buy request was reaching server successfully:

- `game.txt` showed `CMD_880 [server] [propid:200900101]`
- `db.txt` showed `CMD_800 [server]` and `CMD_760`
- `gate.txt` showed:
  - `CMD_800 [client]`
  - `onCMD_760 [client]`
  - `CMD_880 [client]`

But client log showed:

- `Recv ... cmd [836]->[800] len [47]->[75]`
- `Len is Error; [cmd:800]`

This means:

- server did send the bag update
- client dropped the packet before gameplay/UI code saw it

## Latest code fix

The actual bug was in client TCP packet framing in `SimpleNetChannel`.

Problem:

- `Plugins/SimpleNetChannel/.../SimpleTCPManage.cpp` treated one `Socket->Recv()` call as one full gameplay packet
- TCP can merge multiple packets or split one packet across reads
- after a vendor buy, `CMD_800`, `CMD_760`, and `CMD_880` could arrive in the same or partial read
- the plugin then decoded the wrong boundary and rejected `CMD_800` as a bad packet

Fix applied:

- added a TCP receive cache in `SimpleTCPManage`
- parse by `FSimpleBunchHead.len`
- only decrypt and dispatch complete packets
- preserve leftover bytes for the next recv

Files changed for this fix:

- `Plugins/SimpleNetChannel/Source/SimpleNetChannel/Private/Manage/SimpleTCPManage.cpp`
- `Plugins/SimpleNetChannel/Source/SimpleNetChannel/Private/Manage/SimpleTCPManage.h`

Build status:

- `MMOARPGEditor` built successfully after this fix

## Current open risk

Even after TCP framing is fixed, the client still has a weak area:

- there is very little formal C++ handling for `SP_InventorySave (800)` and `SP_InventoryQuery (830)`
- current inventory display still depends a lot on local bag updates and `SP_ItemBuy (880)` path

So if buy succeeds but inventory still does not show, the next likely task is:

1. add explicit client-side handling for `800`
2. add explicit initial bag sync handling for `830`
3. write into `AMMOARPGPlayerState::BagItems`
4. refresh `UI_InventoryBase`

## Fast triage checklist

When the user says "cannot buy" or "buy succeeded but inventory empty", check in this order:

1. client `Saved/Logs/MMOARPG.log`
2. server `login.txt`
3. server `gate.txt`
4. server `center.txt`
5. server `game.txt`
6. server `db.txt`

Key things to distinguish:

- socket connected vs business login succeeded
- buy request sent vs buy request accepted
- server updated bag/gold vs client dropped packet
- client received packet vs UI actually refreshed

## Useful log anchors

Client:

- `Send SP_ItemBuy`
- `Recv SP_ItemBuy`
- `Len is Error`
- `ProtoEnc`

Server:

- `CMD_800`
- `CMD_760`
- `CMD_880`
- `childcmd`
- `create item failed`

## Build command

```powershell
dotnet "D:/ue/UE_5.2/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll" MMOARPGEditor Win64 Development -Project="D:/ueDemo/MMOARPG/MMOARPG.uproject"
```

## Short summary for the next AI

If the user reports the vendor problem again, assume this context:

- server buy path was already mostly working
- the most recent confirmed root cause was client TCP packet framing in `SimpleNetChannel`
- after that fix, the next most likely issue is missing formal client handling for bag sync packets `800/830`
