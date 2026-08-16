# MMORPG Client Runtime

Minimal cross-platform MMORPG client runtime built with **C++ + bgfx**.

The project deliberately avoids a large general-purpose game engine. The runtime owns the game loop, world, ECS, resource lifetime, streaming, and platform integration, while bgfx provides the low-level graphics API abstraction.

## Goals

- Cross-platform client builds.
- Minimal engine/runtime footprint.
- Explicit control over resource loading and unloading.
- Asynchronous asset streaming.
- Server-authoritative MMORPG architecture.
- Shared C++ code where practical.
- Data-driven game content.
- AI-friendly codebase with small, explicit modules.
- No dependency on a scene-centric engine model.

## Non-goals

The project is **not** intended to become a general-purpose game engine.

Avoid adding systems just because a traditional engine normally has them. Every subsystem should justify its existence against the needs of the game.

## Initial Technology Stack

| Area | Technology |
|---|---|
| Language | C++20 initially |
| Build | CMake + Ninja |
| Compiler | clang-cl / MSVC |
| Linker | lld-link |
| Compile cache | sccache |
| Dependencies | FetchContent |
| Rendering | bgfx |
| Window / input | SDL3 |
| Math | glm |
| Physics | Jolt Physics |
| Audio | miniaudio |
| Navigation | Recast & Detour |
| Compression | zstd |
| Textures | KTX2 / Basis Universal |
| Logging | spdlog |
| Tests | Catch2 |
| Formatting | clang-format |
| Static analysis | clang-tidy |

Dependencies should be kept minimal. Prefer small libraries over large frameworks.

## Architecture

```text
                    MMORPG CLIENT
                         |
                +--------+--------+
                |    Game Layer   |
                |                 |
                | Combat          |
                | Quests          |
                | Inventory       |
                | Characters      |
                | NPCs            |
                +--------+--------+
                         |
                +--------+--------+
                |   Game Runtime  |
                |    runtime/     |
                | World           |
                | ECS             |
                | Networking      |
                | Streaming       |
                | Asset Manager   |
                +--------+--------+
                         |
                +--------+--------+
                |  Engine Layer   |
                |    engine/      |
                | Core            |
                | Platform        |
                | Renderer        |
                | Audio           |
                | Physics         |
                | Animation       |
                +--------+--------+
                         |
                +--------+--------+
                |     Libraries   |
                |                 |
                | bgfx / SDL3     |
                | Jolt / etc.     |
                +-----------------+
```

The important architectural boundary is:

```text
Game code
    ↓
Runtime API
    ↓
Engine API
    ↓
Third-party libraries
```

Game systems should not directly depend on bgfx.

## Proposed Repository Layout

```text
.
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
├── LICENSE
├── .gitignore
│
├── cmake/
│   └── ...
│
├── src/
│   ├── engine/              устройства и ресурсы
│   │   ├── core/
│   │   ├── platform/
│   │   ├── renderer/
│   │   ├── animation/
│   │   ├── physics/
│   │   └── audio/
│   │
│   ├── runtime/             мир и правила его существования
│   │   ├── ecs/
│   │   ├── world/
│   │   ├── assets/
│   │   ├── streaming/
│   │   └── networking/
│   │
│   ├── game/
│   │   ├── combat/
│   │   ├── inventory/
│   │   ├── quests/
│   │   ├── characters/
│   │   └── npc/
│   │
│   ├── client/
│   │   └── main.cpp
│   │
│   └── server/
│       └── ...
│
├── tools/
│   ├── asset_compiler/
│   ├── asset_inspector/
│   └── ...
│
├── shaders/
│   └── ...
│
├── assets/
│   ├── raw/
│   └── cooked/
│
├── tests/
│   ├── engine/
│   └── game/
│
└── docs/
    ├── architecture/
    ├── rendering/
    ├── assets/
    └── networking/
```

This structure is a starting point, not a rigid requirement.

## Runtime Principles

### 1. No global scene

Do not build the runtime around a traditional:

```text
Scene
 └── GameObject
      └── Component
```

Instead, the world is data managed by ECS and spatial systems.

### 2. Explicit resource ownership

Resources are identified by stable IDs.

Conceptually:

```cpp
AssetHandle<Mesh> mesh = assets.load<Mesh>(asset_id);
```

The asset manager owns:

- discovery;
- loading;
- decompression;
- CPU-side lifetime;
- GPU upload;
- reference tracking;
- unloading;
- caching.

Game systems should not know where an asset came from.

### 3. Streaming first

The world must be designed around streaming from the beginning.

The client should be able to:

```text
World
  ↓
Visible cells
  ↓
Required assets
  ↓
Async loading
  ↓
GPU upload
  ↓
Active entities
```

Unneeded resources must be unloadable without rebuilding the entire world.

### 4. Server authoritative

The client is not authoritative over important game state.

The server owns:

- character state;
- combat;
- inventory;
- economy;
- quests;
- NPC simulation;
- persistent world state.

The client primarily owns presentation and local prediction.

### 5. Data-driven content

Prefer data:

```text
ItemDefinition
CharacterDefinition
NPCDefinition
QuestDefinition
SkillDefinition
```

over hardcoded content.

Content should eventually be editable without recompiling the engine.

## Rendering

The renderer should expose a small game-oriented API.

Game code should conceptually request:

```cpp
renderer.submit(mesh, material, transform);
```

rather than directly manipulating bgfx objects.

The renderer owns:

- render passes;
- GPU resources;
- materials;
- shaders;
- visibility;
- batching;
- instancing;
- LOD;
- shadows;
- post-processing.

Initial renderer scope should remain deliberately small:

1. Static meshes
2. Materials
3. Textures
4. Camera
5. Basic lighting
6. Directional shadows
7. Skinned meshes
8. GPU instancing
9. LOD
10. Frustum culling

Advanced rendering features should be added only when required by the game.

## Asset Pipeline

Runtime assets should be cooked rather than loaded directly from arbitrary source files.

```text
Raw Assets
    |
    v
Asset Compiler
    |
    v
Cooked Assets
    |
    v
Manifest
    |
    v
Asset Manager
    |
    v
CPU / GPU resources
```

The cooked format should optimize for:

- fast loading;
- streaming;
- compression;
- deterministic builds;
- versioning;
- dependency tracking.

Avoid making the runtime depend on artist-oriented source formats whenever possible.

## World Streaming

The world should be divided into streamable cells/chunks.

```text
+-----+-----+-----+
|     |     |     |
|  A  |  B  |  C  |
|     |     |     |
+-----+-----+-----+
|     |     |     |
|  D  |  E  |  F  |
|     |     |     |
+-----+-----+-----+
|     |     |     |
|  G  |  H  |  I  |
|     |     |     |
+-----+-----+-----+
```

The streaming system determines which cells are:

- unloaded;
- loading;
- loaded;
- active;
- unloading.

Do not couple cell lifetime directly to render-scene lifetime.

## ECS

The ECS should remain simple initially.

Components contain data.

Systems contain behavior.

Example:

```text
Transform
Velocity
Character
Health
NetworkIdentity
Renderable
```

Systems:

```text
MovementSystem
AnimationSystem
CombatSystem
NetworkReplicationSystem
RenderSystem
```

Do not introduce complex ECS abstractions until actual requirements appear.

## Networking

Networking should be isolated from gameplay.

Conceptually:

```text
Network transport
        ↓
Protocol
        ↓
Replication
        ↓
Game state
```

The game layer should not depend directly on sockets.

The networking layer should eventually support:

- connection management;
- snapshots;
- delta compression;
- entity replication;
- client prediction;
- interpolation;
- server reconciliation;
- interest management.

## Client / Server Separation

The project should eventually allow shared simulation code:

```text
shared/
    components
    protocol
    gameplay definitions
    math
```

with separate:

```text
client/
    rendering
    input
    audio
    presentation

server/
    simulation
    persistence
    networking
```

The dedicated server must not depend on the renderer.

## AI Development Rules

This repository is intended to be developed heavily with AI coding agents.

AI agents must follow these rules:

1. Read `README.md` before making architectural changes.
2. Inspect existing code before creating new abstractions.
3. Prefer the smallest implementation that solves the task.
4. Do not introduce a framework when a local implementation is sufficient.
5. Do not modify unrelated systems.
6. Do not silently change public APIs.
7. Add tests for non-trivial logic.
8. Keep engine and game dependencies separated.
9. Do not add dependencies without explaining why they are necessary.
10. Preserve cross-platform compatibility.
11. Avoid premature optimization, but never introduce obviously unnecessary per-frame allocations.
12. Document architectural decisions that affect multiple subsystems.

### AI Agent Workflow

For a non-trivial task:

```text
1. Understand
2. Inspect repository
3. Identify affected subsystem
4. Propose minimal change
5. Implement
6. Build
7. Run tests
8. Check formatting/static analysis
9. Review dependency/API impact
```

Agents should not rewrite large parts of the repository unless explicitly requested.

## Coding Style

- C++20 initially.
- RAII everywhere.
- Prefer value semantics where practical.
- Avoid raw owning pointers.
- Avoid global mutable state.
- Avoid macros except where required by platform/library integration.
- Keep headers lightweight.
- Prefer forward declarations where appropriate.
- Avoid unnecessary templates.
- Avoid hidden allocations in hot paths.
- Use `std::unique_ptr` for exclusive ownership.
- Use `std::shared_ptr` only when shared ownership is actually required.
- Make ownership explicit.

## Performance Principles

The game is intended to support a large number of entities.

Therefore:

- avoid unnecessary allocations;
- avoid per-frame heap allocations;
- prefer contiguous data;
- batch GPU work;
- use instancing;
- use spatial partitioning;
- stream resources asynchronously;
- keep server and client simulation deterministic where practical;
- profile before optimizing complicated code.

Do not optimize based on assumptions.

## First Milestone

The first milestone is **not an MMORPG**.

It is a minimal client runtime capable of:

```text
Application starts
       ↓
Create window
       ↓
Initialize bgfx
       ↓
Initialize renderer
       ↓
Create camera
       ↓
Load one mesh
       ↓
Load one texture
       ↓
Render mesh
       ↓
Move camera
       ↓
Shutdown cleanly
```

After that:

### Milestone 2

```text
Asset Manager
    +
Cooked asset format
    +
Async loading
```

### Milestone 3

```text
ECS
    +
World
    +
Basic entities
```

### Milestone 4

```text
Networking
    +
Dedicated server
    +
Replication
```

### Milestone 5

```text
World streaming
    +
LOD
    +
Interest management
```

### Milestone 6

```text
Gameplay
    +
Combat
    +
NPC
    +
Inventory
    +
Quests
```

## Architectural Decision

The project intentionally chooses:

**C++ + bgfx + custom runtime**

instead of Unity, Unreal, or another large general-purpose engine.

The reason is not that those engines are bad. They are excellent products, but this project requires unusually strong control over:

- runtime lifetime;
- resource lifetime;
- world streaming;
- memory;
- platform abstraction;
- server/client separation;
- generated code;
- project architecture.

The custom runtime should remain small enough that the entire architecture can be understood by both humans and AI agents.

## Current Status

The first milestone is done: the client creates an SDL3 window, initializes bgfx, renders a cleared frame, handles resize, and shuts down cleanly. Verified on Windows with Direct3D 11.

Two subsystems exist so far — `engine/platform` owns the window, `engine/renderer` owns the graphics context and the camera. Neither leaks its library into its public header.

See `docs/code_map.md` for the subsystem map and `docs/build/toolchain.md` for the toolchain.

The next implementation task should be:

> Load and render one static mesh with one texture: a cooked asset format, the shader pipeline, and vertex/index buffer management.

Do not implement ECS, networking, asset streaming, gameplay, or a custom editor yet.

## Definition of Done for the First Commit

The first working commit should:

- configure with CMake;
- build with Ninja;
- launch the client;
- create an SDL3 window;
- initialize bgfx;
- render a frame;
- handle window events;
- close cleanly;
- build without warnings where practical;
- contain no unnecessary engine abstractions.

The first commit should be boring.

That is intentional.
