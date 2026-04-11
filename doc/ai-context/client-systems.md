# Client Systems

## Game instance

`UMMOARPGGameInstance` is the main runtime coordinator.

Key responsibilities visible in `Source/MMOARPG/Core/Common/MMOARPGGameInstance.h`:

- own `FSimpleNetManage* Client`
- hold user and gate status data
- manage local user index and selected character slot
- run QuickTest bootstrap and login flow
- defer `EnterWorld` until travel and world load are ready
- load animation-related data tables for monsters and players

Important fields:

- `QuickTestAccount`
- `QuickTestPassword`
- `QuickTestCharacterSlot`
- `bPendingEnterWorldAfterTravel`
- `bEnterWorldSentForCurrentTravel`

## Network subsystem

`UMMOARPGNetSubsystem` is the protocol routing layer.

Behavior visible in `Source/MMOARPG/Core/Common/MMOARPGNetSubsystem.cpp`:

- creates or reuses the network controller from game instance client
- only binds receive callbacks after secure connection state is ready
- supports one unique handler per protocol
- also supports multicast observers for protocols
- can begin login or gate linking through `BeginLink`

Important implication:

- socket existence is not enough
- channel existence is not enough
- secure connection state is the point where receive binding becomes valid

## Character base

`AMMOARPGCharacterBase` is the shared gameplay character base.

Visible responsibilities in `Source/MMOARPG/Core/Game/Character/Core/MMOARPGCharacterBase.h`:

- action state replication
- interaction component ownership
- shared animation signal entry
- base identifiers such as `ID` and `UserID`
- movement-speed access and replicated action-state transitions

## Practical client files often worth opening early

- `Source/MMOARPG/Core/Common/MMOARPGGameInstance.*`
- `Source/MMOARPG/Core/Common/MMOARPGNetSubsystem.*`
- `Source/MMOARPG/Core/Game/Character/Core/MMOARPGCharacterBase.*`
- `Source/MMOARPG/Core/Game/MMOARPGPlayerState.*`
- `Source/MMOARPG/Inventory/UI_InventoryBase.*`
- `Source/MMOARPG/Inventory/UI_Vendor.*`
- `Saved/Logs/MMOARPG.log`
