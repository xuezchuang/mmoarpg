#pragma once

#include "CoreMinimal.h"
#include "SimpleNetChannelType.h"
#include "MMOARPGType.generated.h"

enum EForcedOfflineType
{
	THERE_ARE_ACCOUNTS_LOGGED_IN_ELSEWHERE,
};

enum ECentralServerLinkType
{
	GAME_PLAYER_LINK,
	GAME_DEDICATED_SERVER_LINK
};

enum ELoginType
{
	LOGIN_DB_SERVER_ERROR,
	LOGIN_SUCCESS,
	LOGIN_ACCOUNT_WRONG,
	LOGIN_WRONG_PASSWORD,
	LOGIN_REPEAT,
};

enum ERegistrationType
{
	ACCOUNT_AND_EMAIL_REPETITION_ERROR,//名字重复
	PLAYER_REGISTRATION_SUCCESS,//注册成功
	SERVER_BUG_WRONG,
};

enum ECheckNameType
{
	UNKNOWN_ERROR,
	NAME_EXIST,
	NAME_NOT_EXIST,
	SERVER_NOT_EXIST,
};

UENUM(BlueprintType)
enum class ECharacterActionState : uint8
{
	NORMAL_STATE	UMETA(DisplayName = "Normal"),
	FIGHT_STATE		UMETA(DisplayName = "Fight"),
	FLIGHT_STATE	UMETA(DisplayName = "Flight"),
	CLIMB_STATE		UMETA(DisplayName = "Climb"),
	SWIMMING_STATE	UMETA(DisplayName = "Swimming"),
};

struct MMOARPGCOMMON_API FMMOARPGUserState
{
	FMMOARPGUserState();

	//当前角色的位置
	FVector Position;

	//当前角色的旋转
	FRotator Rotator;

	//当前角色的前向
	FVector ForwardVector;

	//当前处于DS服务器的ID
	int32 DSID;

	//当前的动作
	ECharacterActionState CharacterActionState;

	//当前处于DS服务器的地址
	//FSimpleAddr DSKey;

	//当前使用的是哪个角色
	int32 CharacterID;
public:
	void Reset();

	FString PositionToString()const;
	FString RotatorToString() const;

	void Init(const FMMOARPGUserState& InUserState);

public:
	bool IsVaild()const { return DSID != INDEX_NONE; }
};

#define USER_MAX_MEMBER	20
#define USER_MAX_PASS	20
#define USER_MAX_KEY	11
#define USER_MAX_NICK	20
#define USER_MAX_ROLE	3
#define MAX_IP_LEN		20

#define  USER_MAX_BAG          150 //最大背包格子数量
#define  USER_INIT_BAG         64 //初始仓库格子数量
#define  USER_MAX_WAREHOUSE    150 //最大仓库格子数量
#define  USER_INIT_WAREHOUSE   64 //初始仓库格子数量
#define  USER_MAX_EQUIP       15  //玩家最大装备数量
#define  MAX_GATE_COUNT       20 //最大网关数量

#define  MAX_SKILL_COUNT    10

#pragma pack(push, 1)
struct S_VECTOR3
{
	int  x;//
	int  y;
	int  z;
	inline void operator=(const S_VECTOR3& pos)
	{
		x = pos.x;
		y = pos.y;
		z = pos.z;
	}
	inline void init()
	{
		x = 0;
		y = 0;
		z = 0;
	}
};
struct MMOARPGCOMMON_API S_LOGIN_GAME
{
	uint8   line;//选择的线路
	uint32  server_connectid;
	uint32  server_clientid; //中心服务器上的客户端ID
	uint32  user_connectid;
	uint32  user_clientid;   //网关服务器上的 客户端ID
	char name[USER_MAX_MEMBER];//账号
	char key[USER_MAX_KEY];    //秘钥
};

struct MMOARPGCOMMON_API S_LOGIN_KEY
{
	char key[USER_MAX_KEY];    //秘钥
};

struct MMOARPGCOMMON_API S_LOGIN_IP
{
	char ip[MAX_IP_LEN];
};

struct MMOARPGCOMMON_API S_LOGIN_NAME
{
	char name[USER_MAX_MEMBER];
};

struct MMOARPGCOMMON_API S_LOGIN_PASS
{
	char pass[USER_MAX_PASS];
};

struct MMOARPGCOMMON_API S_USER_MEMBER_ROLE
{
	uint64		id;//角色id
	uint8		job;//职业
	uint8		sex;//性别
	uint32		clothid; //衣服ID 用于显示主角模型
	uint32		weaponid;//武器ID 用于显示主角武器
	uint16		level;//等级
	char		nick[USER_MAX_NICK];//角色昵称

	inline bool isT() const
	{
		return id > 0;
	}
	inline bool isT_ID(const uint64 value)
	{
		return (id == value);
	}
	inline void reset()
	{
		memset(this, 0, sizeof(S_USER_MEMBER_ROLE));
	}
};

struct MMOARPGCOMMON_API S_COMMAND_BASE
{
	uint32  server_connectid;//服务器上的连接ID
	uint32  server_clientid; //服务器上的客户端ID
	uint32  user_connectid;  //网关服务器上的 连接ID
	uint64  memid;//账号id
};
struct S_COMMAND_GATEBASE
{
	uint32	user_connectid;  //网关服务器上的 连接ID
	int64	memid;//账号id
};
//200 选择角色
struct S_SELECT_ROLE : S_COMMAND_BASE
{
	uint8   roleindex;//角色索引
	uint32  userindex;//玩家索引位置
	uint32  mapid;    //地图id
};
//角色先天属性
struct S_ROLE_BASE_INNATE
{
	uint64		id;    //角色ID
	uint8		job;   //职业
	uint8		sex;   //性别
	char		nick[USER_MAX_NICK]; //昵称	

	inline bool isT() const
	{
		return id > 0;
	}
	inline bool isT_ID(const uint64 value)
	{
		return (value == id);
	}
};
//角色经验 
struct S_ROLE_BASE_EXP
{
	uint16   level; //等级
	uint32   curexp;//当前经验值
};
//荣誉
struct S_ROLE_BASE_HONOR
{
	uint32   medalnum;		//奖牌数量
};
//角色经济
struct S_ROLE_BASE_ECON
{
	uint32     gold;//金币
	uint32     diamonds;//钻石
};
//角色当前状态
struct S_ROLE_BASE_STATUS
{
	uint8		state;
	int16		face; //朝向
	int32		mapid;//当前所在地图ID
	S_VECTOR3	pos;  //当前坐标
	//int32		c_mapid; //保存的地图ID
	//S_VECTOR3	c_pos;//保存坐标

	////进入副本或者退出副本 设置
	//inline void copyMapid(bool isreturn)
	//{
	//	if(isreturn)
	//	{
	//		mapid = c_mapid;
	//		pos = c_pos;
	//	}
	//	else
	//	{
	//		c_mapid = mapid;
	//		c_pos = pos;
	//	}
	//}
};
//生命信息
struct S_ROLE_BASE_LIFE
{
	int32     hp;//当前生命
	int32     mp;//当前魔法值
};
//道具
struct  S_ROLE_PROP_BASE
{
	int32	id;         //道具ID
	uint8	sourcefrom; //来源
	uint8	type;       //类型
	uint16	count;      //数量
	int64	createtime; //创建时间
	int64	uniqueid;   //唯一id
	uint32	money;      //出售价格
};
struct  S_ROLE_PROP_EQUIP
{
	//随机生成的属性
	uint16		p_atk;//物理攻击
	uint16		m_atk;//魔法攻击
	uint16		p_defend;//物理防御
	uint16		m_defend;//魔法防御
	uint8		crit; //暴击
	uint8		dodge;//闪避
	uint16		hp;   //生命
	char		nick[USER_MAX_NICK];//打造装备者的昵称
};
//道具上面的宝石
struct  S_ROLE_PROP_GEM
{
	//宝石是增加固定的属性
	uint8	gem_purple; //紫宝石数量   +物理攻击
	uint8	gem_blue;   //蓝宝石数量   +魔法攻击
	uint8	gem_yellow; //黄宝石数量   +物理防御
	uint8	gem_green;  //绿宝石数量   +魔法防御
	uint8	gem_red;    //红宝石数量   +生命
	inline uint32 GemNum()
	{
		return gem_purple + gem_blue + gem_yellow + gem_green + gem_red;
	}
};
struct  S_ROLE_PROP_TEMP
{
	void* script_prop;//脚本指针地址
};
struct  S_ROLE_PROP
{
	S_ROLE_PROP_BASE   base;  //基础属性
	S_ROLE_PROP_EQUIP  equip; //装备属性
	S_ROLE_PROP_GEM    gem;   //宝石属性
	S_ROLE_PROP_TEMP   temp;

	inline void reset()
	{
		memset(this, 0, sizeof(S_ROLE_PROP));
		temp.script_prop = NULL;
	}
	inline bool IsT()
	{
		if(base.id > 0 && base.count > 0)
		{
			return true;
		}
		return false;
	}
	inline bool isSame(S_ROLE_PROP* prop)
	{
		if(base.id != prop->base.id || base.type != prop->base.type ||
		   base.money != prop->base.money)
			return false;
		return true;
	}
	inline uint32 sendSize()
	{
		uint32 size = sizeof(S_ROLE_PROP_BASE) + sizeof(S_ROLE_PROP_EQUIP) + sizeof(S_ROLE_PROP_GEM);

		return size;
	}

	bool isPropValid();
};
//精灵攻击结构 适用玩家 怪物 NPC 宠物
struct S_SPRITE_ATTACK
{
	int32   hp;
	int32	mp;
	int32	p_atk;//物理攻击
	int32	m_atk;//魔法攻击
	int32	p_defend;//物理防御
	int32	m_defend;//魔法防御
	int32	dodge;//闪避
	int32	crit; //暴击
	int32	dechp;//伤害减免
	int32	speed;//移动速度
	inline void reset()
	{
		memset(this, 0, sizeof(S_SPRITE_ATTACK));
	}
};
/*2-1 背包*/
struct S_ROLE_STAND_BAG
{
	uint8   num;//当前背包数量 最大 150 
	S_ROLE_PROP    bags[USER_MAX_BAG];
	inline void Reset()
	{
		memset(this, 0, sizeof(S_ROLE_STAND_BAG));
	}
	inline S_ROLE_PROP* findEmpty(uint8& pos)
	{
		if(num > USER_MAX_BAG) return nullptr;
		for(uint8 i = 0; i < num; i++)
		{
			if(bags[i].IsT()) continue;
			pos = i;
			return &bags[i];
		}
		return nullptr;
	}
	//inline S_ROLE_PROP* findEmpty(u32 id)
	//{
	//	if (num > USER_MAX_BAG) return nullptr;
	//	for (u8 i = 0; i < num; i++)
	//	{
	//		if (bags[i].IsT()) continue;
	//		if(bag[i].id)
	//		pos = i;
	//		return &bags[i];
	//	}
	//	return nullptr;
	//}
	inline uint8 getEmptyCount()
	{
		uint8 count = 0;
		for(uint8 i = 0; i < num; i++)
		{
			if(bags[i].IsT()) continue;
			count++;
		}
		return count;
	}
};
//2-2 仓库
struct S_ROLE_STAND_WAREHOUSE
{
	uint32  gold;//金币
	uint8   num;//当前仓库数量 最大 150 
	S_ROLE_PROP    warehouses[USER_MAX_WAREHOUSE];
	inline void Reset()
	{
		memset(this, 0, sizeof(S_ROLE_STAND_WAREHOUSE));
	}
	inline S_ROLE_PROP* findEmpty(uint8& pos)
	{
		if(num > USER_MAX_WAREHOUSE) return nullptr;
		for(uint8 i = 0; i < num; i++)
		{
			if(warehouses[i].IsT()) continue;
			pos = i;
			return &warehouses[i];
		}
	}
	inline uint8 getEmptyCount()
	{
		uint8 count = 0;
		for(uint8 i = 0; i < num; i++)
		{
			if(warehouses[i].IsT()) continue;
			count++;
		}
		return count;
	}
};
//角色战斗装备 人物属性面板穿戴的装备
struct S_ROLE_STAND_COMBAT
{
	S_ROLE_PROP   equip[USER_MAX_EQUIP];
	inline void Reset()
	{
		memset(this, 0, sizeof(S_ROLE_STAND_COMBAT));
	}
};
//技能
struct S_ROLE_STAND_SKILL_BASE
{
	uint32   id;
	uint8    level;
	uint8    quick_index;//快捷栏下标

	inline bool isT()
	{
		if(id > 0 && level > 0) return true;
		return false;
	}
};
//技能
struct S_ROLE_STAND_SKILL
{
	S_ROLE_STAND_SKILL_BASE  skill[MAX_SKILL_COUNT];//拥有的技能
};

//主线任务任务
struct S_ROLE_STAND_MAINTASK
{
	uint8    state;      //任务状态0 休闲未开始 1进行中 2任务完成
	uint32   id;         //任务id
	uint8    type;       //任务类型
	uint32   targetid;   //目标任务ID
	uint16   count;      //完成任务数量
	inline bool isT()
	{
		return id > 0;
	}
	inline void reset()
	{
		memset(this, 0, sizeof(S_ROLE_STAND_MAINTASK));
	}
};
//任务
struct S_ROLE_STAND_TASK
{
	S_ROLE_STAND_MAINTASK   main;//1 主线任务
	//2 支线任务
};
//1、角色基础数据
struct S_ROLE_BASE
{
	S_ROLE_BASE_INNATE     innate;//先天属性
	S_ROLE_BASE_EXP        exp;   //经验
	S_ROLE_BASE_HONOR      honor; //荣誉
	S_ROLE_BASE_ECON       econ;  //经济		
	S_ROLE_BASE_STATUS     status;//状态
	S_ROLE_BASE_LIFE       life;   //生命
};
struct S_ROLE_O_BASE
{
	uint16	childcmd;
	uint32	index;
	S_ROLE_BASE_INNATE     innate;
	int16		face;
	S_VECTOR3   pos;
	uint16		level;
	int32		hp;
	int64		memid;
};
//2、角色功能数据
struct S_ROLE_STAND
{
	S_ROLE_STAND_COMBAT    combat;   //角色战斗装备
	S_ROLE_STAND_BAG       bag;      //背包
	S_ROLE_STAND_WAREHOUSE warehouse;//仓库
	S_ROLE_STAND_SKILL     myskill;  //技能
	S_SPRITE_ATTACK        atk;      //玩家基础攻击属性值
	S_ROLE_STAND_TASK      task;     //任务
};

struct S_REBORN_BASE :S_COMMAND_GATEBASE
{
	uint8 kind;//0 原地 1附近城镇复活
	uint32  userindex;  //需要复活的玩家索引
};

struct S_ROBOT_DATA :S_COMMAND_GATEBASE
{
	uint32  robotindex;
};

struct FMonsterDataPacket
{
	uint16  ChildCmd;

	S_ROBOT_DATA   RobotData;

	int32	Id;
	uint8	Dir;
	int32	Hp;

	// 对应 sss(&robot->bc.grid_pos, 8)
	// 服务器写了 8 字节，一般就是两个 int32（XY）；
	// 如果你的 grid_pos 实际是其它结构（比如两个 int16 + padding），请据实修改。
	int32 GridX;
	int32 GridY;
};

struct MMOARPGCOMMON_API FMMOARPGUserData
{
	FMMOARPGUserData() : ID(INDEX_NONE)
	{
	}

	int64 ID;
	S_LOGIN_NAME	Account;
	S_LOGIN_KEY		loingkey;
	S_ROLE_BASE		base; //角色基础数据
	S_ROLE_STAND	stand;//角色功能数据
	S_USER_MEMBER_ROLE role[USER_MAX_ROLE];
	bool IsValid()
	{
		return ID != INDEX_NONE;
	}
	void Reset();
};
//
struct S_MOVE_ROLE
{
	uint32		userindex;
	int16		face;//朝向
	int32		speed;//速度
	S_VECTOR3   targetpos;//目标位置
	S_MOVE_ROLE()
	{
		userindex = 0;
		face = 0;
		speed = 0;
		targetpos.init();
	}
};

struct S_MOVE_ROBOT
{
	uint32		robotindex;
	int32		x;
	int32		y;
	S_MOVE_ROBOT()
	{
		robotindex = 0;
		x = 0;
		y = 0;
	}
};
#pragma pack(pop)



struct MMOARPGCOMMON_API FMMOARPGGateStatus
{
	int32 GateConnetionNum;
	FSimpleAddr GateServerAddrInfo;
};

USTRUCT()
struct MMOARPGCOMMON_API FMMOARPGCharacterAppearance
{
	GENERATED_USTRUCT_BODY()

	FMMOARPGCharacterAppearance()
		:Lv(INDEX_NONE)
		, SlotPosition(INDEX_NONE)
		, LegSize(0.f)
		, WaistSize(0.f)
		, ArmSize(0.f)
		, HeadSize(0.f)
		, ChestSize(0.f)
	{}

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Date;

	UPROPERTY()
	int32 Lv;

	UPROPERTY()
	int32 SlotPosition;

	UPROPERTY()
	float LegSize;

	UPROPERTY()
	float WaistSize;

	UPROPERTY()
	float ArmSize;

	UPROPERTY()
	float HeadSize;

	UPROPERTY()
	float ChestSize;

	void Reset();

public:
	bool IsVaild() const { return SlotPosition != INDEX_NONE; }
};

FORCEINLINE bool operator==(const FMMOARPGCharacterAppearance& Lhs, const FMMOARPGCharacterAppearance& Rhs)
{
	return Lhs.SlotPosition == Rhs.SlotPosition;
}

typedef TArray<FMMOARPGCharacterAppearance> FCharacterAppearances;

USTRUCT(BlueprintType)
struct MMOARPGCOMMON_API FMMOARPGAttributeData
{
	GENERATED_USTRUCT_BODY()

		FMMOARPGAttributeData()
		:BaseValue(0.f)
		, CurrentValue(0.f)
	{}

	UPROPERTY()
		float BaseValue;

	UPROPERTY()
		float CurrentValue;
};

enum class MMOARPGCharacterAttributeType :uint8
{
	ATTRIBUTETYPE_NONE,
	ATTRIBUTETYPE_LEVEL,
	ATTRIBUTETYPE_HEALTH,
	ATTRIBUTETYPE_MAXHEALTH,
	ATTRIBUTETYPE_MANA,
	ATTRIBUTETYPE_MAXMANA,
	ATTRIBUTETYPE_PHYSICSATTACK,
	ATTRIBUTETYPE_MAGICATTACK,
	ATTRIBUTETYPE_PHYSICSDEFENSE,
	ATTRIBUTETYPE_MAGICDEFENSE,
	ATTRIBUTETYPE_ATTACKRANGE,
	ATTRIBUTETYPE_EMPIRICALVALUE,
	ATTRIBUTETYPE_MAXEMPIRICALVALUE,
};

USTRUCT(BlueprintType)
struct FMMOARPGAttributeSlot
{
	GENERATED_USTRUCT_BODY()

	FMMOARPGAttributeSlot(){}
	FMMOARPGAttributeSlot(const TArray<FName> &InSlots)
		:Slots(InSlots)
	{}

	UPROPERTY()
	TArray<FName> Slots;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct MMOARPGCOMMON_API FMMOARPGCharacterAttribute
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FMMOARPGAttributeData Level;

	UPROPERTY()
	FMMOARPGAttributeData Health;

	UPROPERTY()
	FMMOARPGAttributeData MaxHealth;

	UPROPERTY()
	FMMOARPGAttributeData Mana;

	UPROPERTY()
	FMMOARPGAttributeData MaxMana;

	UPROPERTY()
	FMMOARPGAttributeData PhysicsAttack;

	UPROPERTY()
	FMMOARPGAttributeData MagicAttack;

	UPROPERTY()
	FMMOARPGAttributeData PhysicsDefense;

	UPROPERTY()
	FMMOARPGAttributeData MagicDefense;

	UPROPERTY()
	FMMOARPGAttributeData AttackRange;

	UPROPERTY()
	FMMOARPGAttributeData EmpiricalValue;

	UPROPERTY()
	FMMOARPGAttributeData MaxEmpiricalValue;

	UPROPERTY()
	FMMOARPGAttributeData StaminaValue;

	UPROPERTY()
	FMMOARPGAttributeData MaxStaminaValue;

	UPROPERTY()
	FMMOARPGAttributeSlot ComboAttack;

	UPROPERTY()
	FMMOARPGAttributeSlot Skill;

	UPROPERTY()
	FMMOARPGAttributeSlot Limbs;

	UPROPERTY()
	FString SkillAssemblyString;

	void Clear();
};

USTRUCT(BlueprintType)
struct MMOARPGCOMMON_API FDicatedServerControlRange
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector2D UpperLeft;

	UPROPERTY()
	FVector2D UpperRight;

	UPROPERTY()
	FVector2D LowerLeft;

	UPROPERTY()
	FVector2D LowerRight;

	bool IsWithinRange(const FVector &InNewPoint);
};

USTRUCT(BlueprintType)
struct MMOARPGCOMMON_API FMMOARPGDicatedServerInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 ID;

	UPROPERTY()
	FDicatedServerControlRange DicatedServerControlRange;
};

struct MMOARPGCOMMON_API FMMOARPGPlayerRegistInfo
{
	//用户数据
	FMMOARPGUserData UserInfo;

	//捏脸数据
	FMMOARPGCharacterAppearance CAInfo;

	//角色数据
	TMap<int32, FMMOARPGCharacterAttribute> CharacterAttributes;

	//玩家在世界状态的信息
	FMMOARPGUserState UserState;

	void Reset();
	bool IsValid();
};

namespace NetDataAnalysis
{
	void MMOARPGCOMMON_API AnalysisToArrayName(const FString &InRowString, TArray<FName> &OutInfo);

	void MMOARPGCOMMON_API UserDataToString(const FMMOARPGUserData& InUserData, FString& OutString);
	void MMOARPGCOMMON_API StringToUserData(const FString& JsonString, FMMOARPGUserData& OutUserData);

	void MMOARPGCOMMON_API CharacterAppearancesToString(const FCharacterAppearances &InCAs, FString &OutString);
	void MMOARPGCOMMON_API CharacterAppearancesToString(const FMMOARPGCharacterAppearance& InCAs, FString& OutString);
	void MMOARPGCOMMON_API StringToCharacterAppearances(const FString& OutString, FCharacterAppearances& InCAs);
	void MMOARPGCOMMON_API StringToCharacterAppearances(const FString& OutString, FMMOARPGCharacterAppearance& InCAs);

	bool MMOARPGCOMMON_API StringToMMOARPGCharacterAttribute(const FString& OutString, FMMOARPGCharacterAttribute& InCA);
	void MMOARPGCOMMON_API MMOARPGCharacterAttributeToString(const FMMOARPGCharacterAttribute& InCA, FString& OutString);

	bool MMOARPGCOMMON_API StringToMMOARPGCharacterAttribute(const FString& OutString, TMap<int32, FMMOARPGCharacterAttribute>& InCA);
	void MMOARPGCOMMON_API MMOARPGCharacterAttributeToString(const TMap<int32, FMMOARPGCharacterAttribute>& InCA, FString& OutString);

	bool MMOARPGCOMMON_API StringToMMOARPGAttributeSlot(const FString& IntString, FMMOARPGAttributeSlot& OutAS);
	void MMOARPGCOMMON_API MMOARPGAttributeSlotToString(const FMMOARPGAttributeSlot& InAS, FString& OutString);
}

