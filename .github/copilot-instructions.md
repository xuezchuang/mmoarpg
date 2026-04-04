# Copilot Instructions - MMOARPG

Purpose
- Help coding agents become productive quickly in this Unreal Engine 5.2 C++ multiplayer RPG project.
- Prefer precise, minimally invasive edits and preserve Unreal conventions.

Project snapshot
- Engine: Unreal Engine 5.2
- Language: C++ plus Blueprint assets
- Architecture: client-server game with dedicated server target and custom networking plugins

Key paths
- MMOARPG.uproject: project descriptor and plugin enablement
- Source/: primary C++ gameplay code
- Plugins/: custom systems (networking, combat, animation, data access)
- Content/: assets, maps, widgets, data tables
- Config/: engine and game configuration
- Binaries/, Intermediate/, Saved/, DerivedDataCache/: generated outputs, do not edit manually

Build and run (Windows)
- Open in editor: open MMOARPG.uproject
- Build in Visual Studio: open MMOARPG.sln, then build Solution
- Important targets in Source/: MMOARPG.Target.cs, MMOARPGEditor.Target.cs, MMOARPGServer.Target.cs
- Local test loop: compile in Visual Studio, then run Play or Launch in Unreal Editor

Automation notes
- For packaging/automation, use Unreal Automation Tool from your engine install (RunUAT.bat)
- If protocol schemas change, regenerate protobuf outputs with:
	- Plugins/SimpleProtobuf/GenerateProtobuf.bat
	- Plugins/SimpleProtobuf/GenerateProtobufCPP.bat

Architecture boundaries
- Source/MMOARPG/Core/Game/: gameplay framework (GameMode, controllers, character classes)
- Source/MMOARPG/Core/Common/: subsystems and shared runtime state (GameInstance, world subsystems)
- Source/MMOARPG/Core/Component/: reusable actor components
- Source/MMOARPG/NetPlay/: network-play specific game mode and character flow
- Source/MMOARPG/UI/ and Source/MMOARPG/Inventory/: UI and inventory systems
- Source/MMOARPG/Data/ and Source/MMOARPG/DataTable/: gameplay data structures and table-driven data

Important plugins
- Plugins/MMOARPGCommon: shared game and protocol types
- Plugins/SimpleNetChannel: custom network channel and protocol routing
- Plugins/SimpleProtobuf: protocol code generation
- Plugins/SimpleAdvancedAnimation, Plugins/SimpleCombat, Plugins/SimpleHTTP, Plugins/SimpleMySQL: feature modules used by gameplay/services

Code conventions to follow
- Keep Unreal naming prefixes and patterns (A/U/F/E, UPROPERTY, UFUNCTION)
- Prefer existing module boundaries over cross-module coupling
- Maintain replication patterns already in use (for example ReplicatedUsing plus OnRep handlers)
- Avoid editing generated artifacts (.generated.h, files in Intermediate/, Binaries/, Saved/, DerivedDataCache/)
- When changing network behavior, verify whether logic belongs to Unreal replication or SimpleNetChannel protocol handling

Common pitfalls
- Workspace is large; narrow searches to Source/, Plugins/, Config/ when possible
- This repo mixes Unreal replication with custom networking; do not assume one layer owns all multiplayer behavior
- Add full engine source to the workspace only when debugging engine internals; otherwise indexing and search become much slower
- Some plugins may be disabled in MMOARPG.uproject; confirm plugin state before relying on a module

Good starting files
- Source/MMOARPG/Core/Game/Character/Core/MMOARPGCharacterBase.h
- Source/MMOARPG/Core/Common/MMOARPGGameInstance.h
- Source/MMOARPG/MMOARPGGameType.h
- Source/MMOARPG/NetPlay/B2NetGameMode.h
- Plugins/MMOARPGCommon/Source/MMOARPGCommon/Public/MMOARPGType.h
- Source/MMOARPG.Build.cs
- Config/DefaultEngine.ini

Working style for agents
- Make small, targeted changes with clear rationale
- Do not refactor unrelated systems in the same patch
- Prefer linking users to existing project docs rather than duplicating large explanations
- If a task depends on Unreal Editor-only assets or Blueprint graphs, call that out explicitly and provide the exact files to inspect
