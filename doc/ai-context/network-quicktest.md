# Network And QuickTest

## Core network pieces

- transport and packet flow live in `Plugins/SimpleNetChannel`
- gameplay-side network integration lives in `UMMOARPGGameInstance` and `UMMOARPGNetSubsystem`
- login and gate server linking start from the game instance

## QuickTest constraints

Current project rules from `AGENTS.md`:

- QuickTest starts from the current PIE gameplay map
- it must not redirect to `Login` only to bootstrap
- it must not send `SP_EnterWorld` only because the socket connected
- `SP_EnterWorld` is valid only after successful `SP_CharacterResponse`
- `SP_EnterWorld` should be sent after travel completes and only once per travel
- on QuickTest login failure, the client should stop the flow and close the connection

## Fast login triage order

1. `Connect Success`
2. `Recv SP_RCODE`
3. `Send SP_SECURITY`
4. `Recv SP_SECURITY`
5. `Send/Recv SP_LoginResponses`
6. `Send/Recv SP_CharacterLogin`
7. `Send/Recv SP_CharacterSelect`
8. `Recv SP_CharacterResponse`
9. `SP_EnterWorld`

## Important protocol notes

- proto `60000` is heartbeat traffic from `SimpleNetChannel`
- proto `900` is `SP_EnterWorld`
- non-zero `childcmd` in `SP_LoginResponses` means business login failure

## Current default QuickTest credentials in code

From `Source/MMOARPG/Core/Common/MMOARPGGameInstance.h`:

- account: `xuezc_1`
- password: `123456`
- slot: `0`

## Cross-service debugging order

When client and server both matter, compare:

1. `Saved/Logs/MMOARPG.log`
2. `login.txt`
3. `gate.txt`
4. `center.txt`
5. `game.txt`
6. `db.txt`

Do not confuse these three states:

- socket connected
- security handshake succeeded
- business login succeeded
