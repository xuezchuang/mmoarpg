# Copilot Instructions — MMOARPG

Purpose: Help AI assistants quickly understand and work with this Unreal Engine project.

Quick start
- Open the project: `MMOARPG.uproject` (double-click or open in Unreal Editor).
- Native code: open `MMOARPG.sln` in Visual Studio and build the `Solution` or the desired target.
- Editor run: start from the Unreal Editor `Play` or `Launch` buttons for local testing.

Key paths
- `MMOARPG.uproject` — project descriptor
- `Source/` — C++ game code (primary place to modify gameplay systems)
- `Content/` — assets, maps, UI, materials
- `Config/` — project configuration (DefaultEngine.ini, etc.)
- `Binaries/`, `Saved/`, `Intermediate/` — generated outputs; avoid editing these

When to add Unreal Engine (engine) source to the workspace
- Only add the full UE source tree when you need to modify or debug engine internals, recompile the engine, or diagnose low-level engine behavior.
- Downsides: very large repo size, slower IDE indexing/search, increased disk and memory use.
- Recommended approach: keep the project-only workspace by default and temporarily add the engine source as a second workspace folder when needed.

VS Code / IDE tips
- Recommended extensions: Microsoft C/C++ (for IntelliSense), CMake Tools (if used), and any Unreal Engine helper extensions you prefer.
- IntelliSense: point include paths to your engine headers when using a source-built engine, or use the generated project files from the engine's "GenerateProjectFiles" workflow.
- If you add engine source temporarily, ensure the engine version matches the project's expected engine binary/source.

Common build and packaging commands (Windows)
- Build from Visual Studio: open `MMOARPG.sln` -> Build Solution.
- Packaging / automation: use Unreal Automation Tool / `RunUAT.bat` from your engine binaries (project-specific invocation varies).

Agent usage examples
- "How do I build and run the server locally?" — point to `Source/` server modules and `MMOARPG.sln`.
- "Temporarily add engine source and configure IntelliSense" — explain steps to add engine folder to workspace and update include paths.
- "Where are config-driven gameplay values?" — search `Config/` and `Content/` data tables.
- "Create a reusable skill to recompile the project on Windows" — produce a step-by-step skill for building and packaging.

Where to look next
- For deep engine workflows, create an `agent-customization` skill that documents: cloning/matching engine source, GenerateProjectFiles steps, Visual Studio build targets, symbol generation, and common troubleshooting commands.

If you want, I can now create that `agent-customization` skill to codify the engine setup and build flows.
