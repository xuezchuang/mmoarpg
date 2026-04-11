# Server Development Guide (MMOARPG Backend)

> AI 参考文档：从 UE 客户端项目修改服务器代码时的完整上下文。
> 服务器代码路径（WSL）：`\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg`
> Bash 访问路径：`//wsl.localhost/Ubuntu-22.04/root/server_mmorpg`

---

## 仓库结构

```
server_mmorpg/
├── LoginServer/code/    账号登录/注册
├── GateServer/code/     客户端连接路由（纯路由，Phase8已完成）
├── GameServer/code/     游戏逻辑（ECS架构，Phase1-8已全部完成）
├── DBServer/code/       数据库持久化
├── CenterServer/code/   跨服协调（组队、世界状态）
├── engine/              低层网络：WTcpServer、UdpServer、KCP
├── net/                 事件驱动高层网络：EventLoop、TcpServer、TcpSession
├── share/               共享数据/工具
│   ├── CmdDefines.h     所有协议号 CMD_*
│   ├── UserData.h       S_USER_BASE、S_ROBOT_BASE 等核心数据结构
│   ├── WorldData.h      地图、世界相关数据
│   └── CommandData.h    命令数据包结构
├── bin/
│   ├── config/          JSON配置（skill.json, monster.json, item等）
│   ├── *_config/        XML服务器配置（端口、MySQL）
│   └── csv/             静态数据表
└── build/               CMake构建输出（二进制在 bin/）
```

---

## Build 命令（在 WSL Ubuntu 环境执行）

```bash
# 进入 WSL
wsl -d Ubuntu-22.04

# 切到仓库目录
cd /root/server_mmorpg

# 配置（仅首次）
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# 编译全部服务器
cmake --build build

# 编译指定服务器
cmake --build build --target GameServer
cmake --build build --target GateServer
cmake --build build --target LoginServer
cmake --build build --target DBServer
cmake --build build --target CenterServer

# 运行（从项目根目录）
./bin/GameServer
```

> **注意**：编译输出在 `bin/`，不在 `build/`。

---

## 各服务器职责与入口

| 服务器 | 入口文件 | 职责 |
|--------|----------|------|
| LoginServer | `LoginServer/code/LoginServer.cpp` | 账号验证/注册（CMD_10/20/30） |
| GateServer | `GateServer/code/GateServer.cpp` | 客户端路由：登录握手+选角，其余全转发GameServer |
| GameServer | `GameServer/code/GameServer.cpp` | 游戏核心逻辑（ECS架构） |
| DBServer | `DBServer/code/DBServer.cpp` | MySQL读写持久化 |
| CenterServer | `CenterServer/code/CenterServer.cpp` | 跨服：组队、副本协调 |

每个服务器通过 `AppManager::init()` 注册 handler，通过 `app::run()` 启动事件循环。

---

## GameServer ECS 架构（已完成重构）

Phase 1-8 全部完成（最后完成时间 2026-03-20）。

```
GameServer/code/
├── component/     纯数据层（无逻辑）
│   ├── BagComponent.h
│   ├── BuffComponent.h
│   ├── PositionComponent.h
│   ├── SkillComponent.h
│   ├── StatsComponent.h
│   ├── TaskComponent.h
│   └── TeamComponent.h
├── entity/        实体层（持有Component集合）
│   ├── IEntity.h
│   ├── Player.h/.cpp    包装 S_USER_BASE
│   ├── Monster.h/.cpp   包装 S_ROBOT_BASE
│   └── Npc.h/.cpp
├── service/       纯业务逻辑（不感知网络包格式）
│   ├── InventoryService.h/.cpp
│   ├── TeamService.h/.cpp
│   ├── QuestService.h/.cpp
│   ├── CombatService.h/.cpp
│   ├── SkillService.h/.cpp
│   ├── MonsterService.h/.cpp
│   ├── BroadcastService.h/.cpp
│   ├── CopyService.h/.cpp
│   └── WareHouseService.h/.cpp
├── handler/       薄协议层（解包→找实体→调Service→回包）
│   ├── BagHandler.h/.cpp
│   ├── SkillHandler.h/.cpp
│   ├── MoveHandler.h/.cpp
│   ├── TeamHandler.h/.cpp
│   ├── TaskHandler.h/.cpp
│   ├── MonsterHandler.h/.cpp
│   ├── WareHouseHandler.h/.cpp
│   └── [其他Handler.h（接口声明）]
├── event/         EventBus（跨系统解耦）
│   ├── EventBus.h/.cpp
│   ├── GameEvents.h     OnMonsterKilled/OnItemAdded/OnQuestUpdated等
│   └── EventSubscriptions.h/.cpp
├── system/        帧驱动系统（ECS Update Loop）
│   ├── SystemManager.h/.cpp
│   ├── BuffSystem.h/.cpp
│   ├── CooldownSystem.h/.cpp
│   ├── AiSystem.h/.cpp
│   ├── BroadcastSystem.h/.cpp
│   └── HeartbeatSystem.h/.cpp
└── [旧 App* 文件]  AppBag/AppSkill/AppMove等（已被Handler+Service替代，逐步保留兼容）
```

### 数据流向

```
客户端 CMD → GateServer(Router转发) → GameServer
  → Handler（解包/校验）
  → Service（业务逻辑）
  → EventBus（跨系统通知）
  → System（帧驱动更新）
  → 回包给客户端 / 转发给DBServer
```

---

## GateServer 架构（Phase 8 完成后）

```
GateServer/code/
├── AppLogin.cpp       登录握手（CMD_111, CMD_HEART）
├── AppSelectRole.cpp  选角（CMD_200-500）
├── AppEntryWorld.cpp  进入世界/切图/切线路
├── AppGate.cpp        服务器心跳/注册（CMD_40/50）
├── AppGlobal.cpp      初始化
└── Router.h/.cpp      统一路由转发（替代原10+个App*文件）
```

`Router` 把所有非登录/非选角的 CMD 自动转发给对应 GameServer。

---

## 协议号速查（share/CmdDefines.h）

| 范围 | 用途 |
|------|------|
| CMD_10/20/30 | 账号登录/注册/更新时间（LoginServer） |
| CMD_40/50 | 服务器注册（GateServer↔GameServer） |
| CMD_90 | 心跳 |
| CMD_100 | 登录游戏、获取角色列表 |
| CMD_200 | 选择角色开始游戏 |
| CMD_300/400 | 创建/删除角色 |
| CMD_500-602 | 角色数据同步、掉线 |
| CMD_700-770 | 状态更新（HP/MP/坐标/经验/等级/经济/攻击属性） |
| CMD_800-890 | 背包/装备/出售/购买/合成 |
| CMD_900-902 | 进入世界/切换地图/换线 |
| CMD_910-980 | 仓库操作 |
| CMD_1000/1100/1200 | 移动（自己/他人/速度变化） |
| CMD_2000 | 聊天 |
| CMD_3000-3200 | 技能（释放/出招/伤害） |
| CMD_4000-4200 | 玩家死亡 |
| CMD_5000 | Buff |
| CMD_6000-6300 | 掉落（Drop） |
| CMD_8000+ | 怪物数据 |
| Proto 60000 (UE侧) | SimpleNetChannel 心跳 |
| Proto 900 (UE侧) | SP_EnterWorld |

---

## 关键共享数据结构（share/）

| 文件 | 内容 |
|------|------|
| `UserData.h` | `S_USER_BASE`（玩家数据核心）、`S_ROBOT_BASE`（怪物/NPC） |
| `CmdDefines.h` | 全部 CMD_* 协议号 |
| `CommandData.h` / `CommandData2.h` | 命令数据包 struct |
| `WorldData.h` | 地图、世界坐标相关 |
| `RoleData.h` | 角色扮演相关数据 |
| `SkillData.h` | 技能数据结构 |
| `TaskData.h` | 任务数据结构 |
| `TeamData.h` | 组队数据结构 |
| `PropData.h` | 道具/物品属性数据 |
| `GameConstants.h` | 游戏常量 |

---

## 日志位置

```
# 运行日志（按日期目录）
//wsl.localhost/Ubuntu-22.04/root/server_mmorpg/build/bin/<YYYY-MM-DD>/
  ├── login.txt
  ├── gate.txt
  ├── center.txt
  ├── game.txt
  └── db.txt
```

日期目录动态变化，每次调试时需动态查找最新日期目录。

---

## 命名空间

| 命名空间 | 所属 | 用途 |
|----------|------|------|
| `app::` | 共享 | 游戏枚举、结构体、协议定义 |
| `ecs::` | GameServer | Entity/Component 层 |
| `service::` | GameServer | Service 层单例 |
| `sys::` | GameServer | ECS System 层 |
| `event::` | GameServer | EventBus 和事件类型 |
| `net::` | net/ | 网络框架 |

---

## 服务器间通信

通过 `ITcpServer`/`ITcpClient`（`engine/INetBase.h`）TCP 通信。  
每个服务器通过 `registerCommand(cmd, container)` 注册处理器。  
`IContainer` 接口（`engine/IContainer.h`）定义 `onServerCommand` 和 `onClientCommand`。

---

## 修改代码后的工作流

1. 用工具直接编辑 `//wsl.localhost/Ubuntu-22.04/root/server_mmorpg/` 下的文件
2. 在 WSL 中编译：`cmake --build build --target <ServerName>`
3. 检查编译错误，修复后重新编译
4. 重启对应服务器进程验证
5. 查看 `build/bin/<日期>/` 下的对应日志

---

## 客户端-服务器协议对照（UE侧 vs 服务端）

| UE 客户端（SimpleNetChannel Proto） | 服务端 CMD | 说明 |
|-------------------------------------|------------|------|
| SP_LoginRequest | CMD_10 | 账号登录 |
| SP_CharacterResponse | CMD_100/200 | 选角数据 |
| SP_EnterWorld (Proto 900) | CMD_900 | 进入世界 |
| Proto 60000 | CMD_90 | 心跳 |

> **重要**：Socket连接成功 ≠ 业务登录成功，需等握手和业务响应都确认。  
> SP_EnterWorld 只能在 SP_CharacterResponse 成功、地图Travel完成后发送。
