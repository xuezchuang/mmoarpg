# AI Context Pack

This folder is a small, curated context pack for AI assistants working in this repository.

It is useful only when it stays:

- short
- accurate
- tied to real source files
- updated when code paths change

It is not useful if it becomes a second, stale copy of the codebase.

## Recommended read order

1. `AGENTS.md`
2. `doc/ai-context/README.md`
3. one focused file below based on the task
4. the real source files involved in the task

## Files

- `project-overview.md`
  - high-level project layout, modules, and plugin map
- `client-systems.md`
  - main client runtime entry points and gameplay-side systems
- `network-quicktest.md`
  - login flow, packet routing, and QuickTest constraints
- `item-inventory.md`
  - item enums, inventory-related code paths, and practical touch points
- `server-backend.md`
  - live server repo location, build flow, server layout, and backend entry points

## Maintenance rule

When a doc here disagrees with code, trust the code and update the doc.
