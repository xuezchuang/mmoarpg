# 服务器源码架构记忆

## 服务器目录结构

```
server_mmorpg/
├── CenterServer/     # 中心服务器 - 管理用户GameServer注册
├── LoginServer/      # 登录服务器 - 处理账号登录/注册
├── GateServer/      # 网关服务器 - 客户端入口,转发消息
├── DBServer/        # 数据库服务器 - MySQL数据持久化
├── GameServer/      # 游戏服务器 - 游戏逻辑处理
│   └── code/
│       ├── entity/      # 实体 (Player, Monster, Npc)
│       ├── component/    # 组件 (Bag, Buff, Position, Skill, Stats, Task, Team)
│       ├── system/       # 系统 (AI, Broadcast, Buff, Cooldown, Heartbeat)
│       ├── service/      # 服务 (Combat, Copy, Monster, Skill, Team, Quest, WareHouse, Inventory)
│       └── handler/      # 协议处理 (Bag, Move, Skill, Task, Team, Monster)
├── net/              # 网络底层 (ProtocolStream)
└── share/            # 共享函数 (ShareFunction)
```

## 服务器通信架构

```
客户端
  │
  ├─LoginServer (登录/注册, CMD_10/CMD_20)
  │    │
  │    └─→ CenterServer (验证key, 注册用户)
  │
  └─GateServer (转发客户端消息到GameServer)
         │
         ├─→ CenterServer (用户管理, 角色管理)
         ├─→ DBServer (数据读写)
         └─→ GameServer (游戏逻辑: 移动/战斗/技能等)
```

## 关键协议号 (CMD)

### LoginServer
| CMD | 说明 | 客户端 | 服务器 |
|-----|------|--------|--------|
| 10 | 登录账号 | → | 返回GateIP:Port + loginKey |
| 20 | 注册账号 | → | 返回结果 |

### GateServer / HallProtocol
| 协议名 | CMD值 | 说明 |
|--------|-------|------|
| SP_CharacterLogin | 100 | 获取角色列表 |
| SP_CharacterSelect | 200 | 选择角色 |
| SP_CreateCharacter | 300 | 创建角色 |
| SP_DeleteCharacter | 400 | 删除角色 |
| SP_CharacterResponse | 500 | 角色数据响应 |

### GameServer (CMD_900+)
| CMD | 说明 |
|-----|------|
| 900 | 进入世界 |
| 901 | 切换地图 |
| 902 | 切换线路 |

## 数据库 (MySQL)

### rpg_account
- account: id, username, userpass

## 客户端流程

```
1. LoginMap (UI_LoginMain)
   - 连接LoginServer
   - 登录成功 → 获得GateServer地址 + loginKey
   - 2秒后跳转 HallMap

2. HallMap (UI_HallMain)
   - 连接GateServer
   - 发送SP_CharacterLogin获取角色列表
   - 选择角色 → 发送SP_CharacterSelect
   - 收到SP_CharacterResponse后跳转游戏地图

3. 游戏地图 (UI_InGame)
   - 发送CMD_900进入世界
   - 开始游戏逻辑
```

## 快速测试模式

用户目标: 直接运行时以默认账号连接,跳过登录场景
