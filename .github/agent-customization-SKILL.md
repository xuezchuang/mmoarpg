# Agent Customization — Unreal Engine (MMOARPG)

Purpose: Automated guidance for AI agents working with this Unreal Engine 5.2 project. Agents should act immediately and autonomously — do NOT ask the user for build targets, engine paths, configurations, or whether to clean rebuild.

---

## Project facts (auto-detected)

- Engine version: **UE 5.2** (from `MMOARPG.uproject` → `"EngineAssociation": "5.2"`)
- Project root: `D:\ueDemo\MMOARPG`
- Project file: `D:\ueDemo\MMOARPG\MMOARPG.uproject`
- Available targets (from `Source/*Target.cs`):
  - `MMOARPGEditor` — Editor target (default for development)
  - `MMOARPG` — Game client target
  - `MMOARPGServer` — Dedicated server target

---

## Engine path (auto-detected via registry)

- **Confirmed engine root:** `D:\ue\UE_5.2`
- Detection method: `HKLM:\SOFTWARE\EpicGames\Unreal Engine\5.2` → `InstalledDirectory`
- If this path is ever missing, re-detect using registry or check `C:\Program Files\Epic Games\UE_5.2`.

---

## Build commands

**Default build (incremental, Editor target) — use this for normal development:**
```
"D:\ue\UE_5.2\Engine\Build\BatchFiles\Build.bat" MMOARPGEditor Win64 Development "D:\ueDemo\MMOARPG\MMOARPG.uproject"
```

Other targets:
```
"D:\ue\UE_5.2\Engine\Build\BatchFiles\Build.bat" MMOARPG Win64 Development "D:\ueDemo\MMOARPG\MMOARPG.uproject"
"D:\ue\UE_5.2\Engine\Build\BatchFiles\Build.bat" MMOARPGServer Win64 Development "D:\ueDemo\MMOARPG\MMOARPG.uproject"
```

Generate project files (only needed after `*.Build.cs` changes):
```
"D:\ue\UE_5.2\GenerateProjectFiles.bat" -projectfiles -project="D:\ueDemo\MMOARPG\MMOARPG.uproject" -game -engine
```

---

## Agent build behavior — CRITICAL RULES

1. **NEVER ask the user** for engine path, build target, configuration, or whether to do a clean rebuild.
2. **Always use incremental builds** by default. Do not delete `Binaries/` or `Intermediate/` unless the user explicitly says "clean rebuild" or "重新编译".
3. **Auto-detect the engine path** using the order above. If none of the paths exist, report the error and ask for the engine path only at that point.
4. **Default target is `MMOARPGEditor`** with `Development` configuration unless the user specifies otherwise.
5. When the user says "compile" or "编译" → run Build.bat immediately, capture output, parse errors, fix them, then recompile.
6. When the user says "fix bug" → read source files, locate the bug, edit the file, then trigger an incremental build automatically.

---

## Clean rebuild (only on explicit user request)

Only perform when user says "clean rebuild" or "重新编译". Never do this automatically.
```powershell
Remove-Item -Recurse -Force "D:\ueDemo\MMOARPG\Binaries"
Remove-Item -Recurse -Force "D:\ueDemo\MMOARPG\Intermediate"
# Then run Build.bat as above
```

---

## Troubleshooting

- Build logs: `D:\ueDemo\MMOARPG\Saved\Logs\`
- Missing includes / module not found → check `*.Build.cs` for missing `PublicDependencyModuleNames`
- Stale binaries after module changes → suggest clean rebuild (ask user first)
- VS components missing → need MSVC toolset + Windows SDK + .NET desktop development workload
