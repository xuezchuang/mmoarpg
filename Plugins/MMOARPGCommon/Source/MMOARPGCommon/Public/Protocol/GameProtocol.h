#pragma once
#include "SimpleProtocolsDefinition.h"

DEFINITION_SIMPLE_PROTOCOLS(RoleBaseInfo, 700)
DEFINITION_SIMPLE_PROTOCOLS(VendorBuy, 880)
DEFINITION_SIMPLE_PROTOCOLS(VendorSell, 870)
DEFINITION_SIMPLE_PROTOCOLS(EnterWorld, 900)
DEFINITION_SIMPLE_PROTOCOLS(UpdatePos, 1000)
DEFINITION_SIMPLE_PROTOCOLS(UpdateOtherPos, 1100)


DEFINITION_SIMPLE_PROTOCOLS(MonsterData,         8000)  // �������ݣ���������Ϣ/λ�õȣ�
DEFINITION_SIMPLE_PROTOCOLS(MonsterAck,          8100)  // ���ﷵ��/ȷ��
DEFINITION_SIMPLE_PROTOCOLS(MonsterHP,           8200)  // ������������
DEFINITION_SIMPLE_PROTOCOLS(MonsterState,        8300)  // ����״̬����Idle/Chase/Back�ȣ�
DEFINITION_SIMPLE_PROTOCOLS(MonsterMove,         8400)  // �����ƶ�
DEFINITION_SIMPLE_PROTOCOLS(MonsterAttack,       8500)  // �������/����
DEFINITION_SIMPLE_PROTOCOLS(MonsterSkillDamage,  8700)  // �����˺����
DEFINITION_SIMPLE_PROTOCOLS(MonsterBuff,         8800)  // ���� Buff �仯


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