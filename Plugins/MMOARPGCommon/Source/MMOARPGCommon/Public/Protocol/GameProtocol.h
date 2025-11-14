#pragma once
#include "SimpleProtocolsDefinition.h"
DEFINITION_SIMPLE_PROTOCOLS(RoleBaseInfo,        700)   // 其他角色基础信息
DEFINITION_SIMPLE_PROTOCOLS(RoleHP,              710)   // 更新 HP
DEFINITION_SIMPLE_PROTOCOLS(RoleMP,              720)   // 更新 MP
DEFINITION_SIMPLE_PROTOCOLS(RoleState,           730)   // 玩家状态（Idle/Move/Fight等）

DEFINITION_SIMPLE_PROTOCOLS(RolePosition,        731)   // 坐标同步
DEFINITION_SIMPLE_PROTOCOLS(RoleExp,             740)   // 经验更新
DEFINITION_SIMPLE_PROTOCOLS(RoleLevel,           750)   // 等级更新
DEFINITION_SIMPLE_PROTOCOLS(RoleCurrency,        760)   // 经济（金币/钻石）
DEFINITION_SIMPLE_PROTOCOLS(RoleAttributes,      770)   // 战斗属性更新

DEFINITION_SIMPLE_PROTOCOLS(InventorySave,       800)   // 背包道具保存
DEFINITION_SIMPLE_PROTOCOLS(EquipmentSave,       810)   // 战斗装备保存
DEFINITION_SIMPLE_PROTOCOLS(EquipmentQuery,      820)   // 查询他人装备
DEFINITION_SIMPLE_PROTOCOLS(InventoryQuery,      830)   // 获取背包数据
DEFINITION_SIMPLE_PROTOCOLS(InventorySort,       840)   // 整理背包
DEFINITION_SIMPLE_PROTOCOLS(EquipItem,           850)   // 穿上装备
DEFINITION_SIMPLE_PROTOCOLS(UnequipItem,         860)   // 脱下装备
DEFINITION_SIMPLE_PROTOCOLS(ItemSell,            870)   // 出售道具
DEFINITION_SIMPLE_PROTOCOLS(ItemBuy,             880)   // 购买道具
DEFINITION_SIMPLE_PROTOCOLS(ItemCraft,           890)   // 合成 / 打造

DEFINITION_SIMPLE_PROTOCOLS(EnterWorld,			 900)
DEFINITION_SIMPLE_PROTOCOLS(StorageQuery,        910)   // 申请仓库数据
DEFINITION_SIMPLE_PROTOCOLS(StorageDepositGold,  920)   // 背包 → 仓库存金币
DEFINITION_SIMPLE_PROTOCOLS(StorageUpdateGold,   921)   // 仓库金币更新
DEFINITION_SIMPLE_PROTOCOLS(StorageDepositItem,  930)   // 背包 → 仓库存放道具
DEFINITION_SIMPLE_PROTOCOLS(StorageUpdateItem,   931)   // 仓库道具更新
DEFINITION_SIMPLE_PROTOCOLS(StorageSwap,         940)   // 仓库-仓库道具交换

DEFINITION_SIMPLE_PROTOCOLS(StorageWithdrawGold, 960)   // 取出金币
DEFINITION_SIMPLE_PROTOCOLS(StorageWithdrawItem, 970)   // 取出道具
DEFINITION_SIMPLE_PROTOCOLS(StorageLog,          980)   // 操作日志 / 更新事件



DEFINITION_SIMPLE_PROTOCOLS(SelfMove,           1000)   // 自己移动
DEFINITION_SIMPLE_PROTOCOLS(OtherMove,          1100)   // 其他玩家移动
DEFINITION_SIMPLE_PROTOCOLS(RoleSpeed,          1200)   // 速度变化
DEFINITION_SIMPLE_PROTOCOLS(Chat,               2000)   // 聊天


DEFINITION_SIMPLE_PROTOCOLS(SkillUse,           3000)   // 技能使用
DEFINITION_SIMPLE_PROTOCOLS(SkillCast,          3100)   // 技能出招
DEFINITION_SIMPLE_PROTOCOLS(SkillDamage,        3200)   // 技能伤害结果

DEFINITION_SIMPLE_PROTOCOLS(RoleDead,           4000)   // 玩家死亡
DEFINITION_SIMPLE_PROTOCOLS(RoleRevive,         4100)   // 复活（可选）
DEFINITION_SIMPLE_PROTOCOLS(RoleHurt,           4200)   // 受击（可选）

DEFINITION_SIMPLE_PROTOCOLS(RoleBuff,           5000)   // Buff 改变


DEFINITION_SIMPLE_PROTOCOLS(TeamCreate,         7000)
DEFINITION_SIMPLE_PROTOCOLS(TeamInvite,         7010)
DEFINITION_SIMPLE_PROTOCOLS(TeamAccept,         7020)
DEFINITION_SIMPLE_PROTOCOLS(TeamReject,         7030)
DEFINITION_SIMPLE_PROTOCOLS(TeamLeave,          7040)
DEFINITION_SIMPLE_PROTOCOLS(TeamKick,           7050)
DEFINITION_SIMPLE_PROTOCOLS(TeamApply,          7060)
DEFINITION_SIMPLE_PROTOCOLS(TeamApplyAccept,    7070)
DEFINITION_SIMPLE_PROTOCOLS(TeamApplyReject,    7080)
DEFINITION_SIMPLE_PROTOCOLS(TeamTransferLeader, 7090)
DEFINITION_SIMPLE_PROTOCOLS(TeamUpdateMembers,  7100)
DEFINITION_SIMPLE_PROTOCOLS(TeamLeaderBroadcast,7110)
DEFINITION_SIMPLE_PROTOCOLS(TeamSelfJoin,       7120)



DEFINITION_SIMPLE_PROTOCOLS(DungeonSoloStart,   7200)
DEFINITION_SIMPLE_PROTOCOLS(DungeonMultiStart,  7300)
DEFINITION_SIMPLE_PROTOCOLS(DungeonEnter,       7301)
DEFINITION_SIMPLE_PROTOCOLS(DungeonLeave,       7400)
DEFINITION_SIMPLE_PROTOCOLS(DungeonDisband,     7401)

DEFINITION_SIMPLE_PROTOCOLS(TeamSyncToGame,     7500)
DEFINITION_SIMPLE_PROTOCOLS(PlayerTeamSyncToGS, 7600)

DEFINITION_SIMPLE_PROTOCOLS(MonsterData,         8000)  // 怪物数据（含基础信息/位置等）
DEFINITION_SIMPLE_PROTOCOLS(MonsterAck,          8100)  // 怪物返回/确认
DEFINITION_SIMPLE_PROTOCOLS(MonsterHP,           8200)  // 怪物生命更新
DEFINITION_SIMPLE_PROTOCOLS(MonsterState,        8300)  // 怪物状态（如Idle/Chase/Back等）
DEFINITION_SIMPLE_PROTOCOLS(MonsterMove,         8400)  // 怪物移动
DEFINITION_SIMPLE_PROTOCOLS(MonsterAttack,       8500)  // 怪物出招/攻击
DEFINITION_SIMPLE_PROTOCOLS(MonsterSkillDamage,  8700)  // 技能伤害结果
DEFINITION_SIMPLE_PROTOCOLS(MonsterBuff,         8800)  // 怪物 Buff 变化

DEFINITION_SIMPLE_PROTOCOLS(QuestTrigger,       9000) // 触发任务（可选含剧情）
DEFINITION_SIMPLE_PROTOCOLS(QuestSubmit,        9100) // 提交任务
DEFINITION_SIMPLE_PROTOCOLS(QuestUpdate,        9110) // 任务进展更新
