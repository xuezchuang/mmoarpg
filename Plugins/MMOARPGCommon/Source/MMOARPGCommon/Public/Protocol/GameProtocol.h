#pragma once
#include "SimpleProtocolsDefinition.h"

DEFINITION_SIMPLE_PROTOCOLS(RoleBaseInfo, 700)
DEFINITION_SIMPLE_PROTOCOLS(VendorBuy, 880)
DEFINITION_SIMPLE_PROTOCOLS(VendorSell, 870)
DEFINITION_SIMPLE_PROTOCOLS(EnterWorld, 900)
DEFINITION_SIMPLE_PROTOCOLS(UpdatePos, 1000)
DEFINITION_SIMPLE_PROTOCOLS(UpdateOtherPos, 1100)


DEFINITION_SIMPLE_PROTOCOLS(MonsterData,         8000)  // 怪物数据（含基础信息/位置等）
DEFINITION_SIMPLE_PROTOCOLS(MonsterAck,          8100)  // 怪物返回/确认
DEFINITION_SIMPLE_PROTOCOLS(MonsterHP,           8200)  // 怪物生命更新
DEFINITION_SIMPLE_PROTOCOLS(MonsterState,        8300)  // 怪物状态（如Idle/Chase/Back等）
DEFINITION_SIMPLE_PROTOCOLS(MonsterMove,         8400)  // 怪物移动
DEFINITION_SIMPLE_PROTOCOLS(MonsterAttack,       8500)  // 怪物出招/攻击
DEFINITION_SIMPLE_PROTOCOLS(MonsterSkillDamage,  8700)  // 技能伤害结果
DEFINITION_SIMPLE_PROTOCOLS(MonsterBuff,         8800)  // 怪物 Buff 变化


//DEFINITION_SIMPLE_PROTOCOLS(UpdateLoginCharacterInfoRequests, 18)
//DEFINITION_SIMPLE_PROTOCOLS(UpdateLoginCharacterInfoResponses, 19)
//DEFINITION_SIMPLE_PROTOCOLS(GetCharacterDataRequests, 24)
//DEFINITION_SIMPLE_PROTOCOLS(GetCharacterDataResponses, 25)
//DEFINITION_SIMPLE_PROTOCOLS(CharacterUpgradeLevelRequests, 30)
//DEFINITION_SIMPLE_PROTOCOLS(CharacterUpgradeLevelResponses, 31)
//DEFINITION_SIMPLE_PROTOCOLS(UpdateAttributeRequests, 32)
//DEFINITION_SIMPLE_PROTOCOLS(UpdateAttributeaResponses, 33)
//DEFINITION_SIMPLE_PROTOCOLS(UpdateSkillAssemblyRequests, 36)
//DEFINITION_SIMPLE_PROTOCOLS(UpdateSkillAssemblyResponses, 37)