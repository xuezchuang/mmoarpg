#pragma once
#include "Engine/DataTable.h"
#include "FItemData.generated.h"

static const FName UserDefinedStructName = TEXT("UserDefinedStruct");

/** C++ E_ItemType */
UENUM(BlueprintType)
enum class E_ItemType : uint8
{
	None,
	Armor,
	Weapon,
	Shield,
	Food,
	Potion,
	Crafting_Ingredient,
	Quest_Item,
	Other,
	Currency,
	Bow,
	Arrow,
	Accessories,
	Horse_Saddle,
	Horse_Armor,
	Horse_Reins,
	Horse_Wings,
	Horse_Horn,
	Glider,
	Mining_Tool,
	Logging_Tool,
	Fishing_Tool,
};

/** C++ E_ItemRarity */
UENUM(BlueprintType)
enum class E_ItemRarity : uint8
{
	None,
	Common,
	Superior,
	Epic,
	Legendary,
	Consumable,
	Usable,
};

/** C++ E_ItemSlot */
UENUM(BlueprintType)
enum class E_ItemSlot : uint8
{
	None,
	Head,
	Gloves,
	Neck,
	Chestplate,
	Legs,
	Boots,
	Weapon,
	Shield,
	Pocket1,
	Pocket2,
	Pocket3,
	Pocket4,
	Bow,
	Arrow,
	Talisman,
	Ring,
	Bracelet,
	Saddle,
	Barding,
	Reins,
	Mount,
	HHorn,
	Glider,
	Mining,
	Logging,
	Fishing,
};

/** C++ E_WeaponCategory */
UENUM(BlueprintType)
enum class E_WeaponCategory : uint8
{
	None,
	Sword,
	Warhammer,
	Axe,
	Mace,
	Bow,
	Arrow,
	Greatsword,
	SwordShield,
	DualWield,
	Spear,
	Blunt,
	MiningAxe,
	LoggingAxe,
	FishingRod,
	Katana,
};

/** C++ E_InventoryPanels */
UENUM(BlueprintType)
enum class E_InventoryPanels : uint8
{
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8,
	P9,
	P10,
	None,
};

/** C++ S_ItemDesc */
USTRUCT(BlueprintType)
struct FS_ItemDesc
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "ID"))
	FString ID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Name"))
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Description"))
	FText Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (MultiLine = "true", DisplayName = "Readable Text"))
	TArray<FText> ReadableText;
};

/** C++ E_StatType */
UENUM(BlueprintType)
enum class E_StatType : uint8
{
	None,
	Health,
	Stamina,
	Mana,
	PhysicalDmg,
	Armor,
	CriticalRate,
	CriticalDamage,
	PhAttackSpeed,
	Block,
	AStaminaCost,
	MagicalDmg,
	CastingSpeed,
	MountMaxSpeed,
	MountMaxStamina,
	MountKickDamage,
	MountAcceleration,
	MountDeceleration,
	MountKickStaminaCost,
	RangeDmg,
	RnAttackSpeed,
	FireDamage,
	PoisonDamage,
	ExplosiveDamage,
	EffectDuration,
};

/** C++ FF_Modifier */
USTRUCT(BlueprintType)
struct FF_Modifier
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Type", MakeStructureDefaultValue = "NewEnumerator0"))
	E_StatType Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Value"))
	float Value;
};

/** C++ E_StatCategory */
UENUM(BlueprintType)
enum class E_StatCategory : uint8
{
	None,
	Health,
	MaxHealth,
	Stamina,
	MaxStamina,
	Armor,
	Damage,
	AttackSpeed,
};

/** C++ FS_ConsumableAction */
USTRUCT(BlueprintType)
struct FS_ConsumableAction
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Category", MakeStructureDefaultValue = "NewEnumerator0"))
	uint8 Category;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Stat", MakeStructureDefaultValue = "NewEnumerator8"))
	E_StatCategory Stat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Value"))
	float Value;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Number of Repetitions"))
	int32 NumberofRepetitions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Duration"))
	float Duration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Additional Effects", MakeStructureDefaultValue = "NewEnumerator2"))
	uint8 AdditionalEffects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Consumable Cooldown"))
	float ConsumableCooldown;
};

/** C++ FS_ItemDurability */
USTRUCT(BlueprintType)
struct FS_ItemDurability
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Use Durability"))
	bool UseDurability;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Current Durability"))
	int32 CurrentDurability;
};

/** C++ FS_ItemStats */
USTRUCT(BlueprintType)
struct FS_ItemStats
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Weight", MakeStructureDefaultValue = "0.000000"))
	float Weight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Value", MakeStructureDefaultValue = "0.000000"))
	float Value;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Stats"))
	TArray<FF_Modifier> Stats;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Required Level", MakeStructureDefaultValue = "0"))
	int32 RequiredLevel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Consumable Action"))
	FS_ConsumableAction ConsumableAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Durability"))
	FS_ItemDurability Durability;
};

/** C++ FS_ItemStacks */
USTRUCT(BlueprintType)
struct FS_ItemStacks
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Stackable"))
	bool Stackable;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Quantity", MakeStructureDefaultValue = "1"))
	int32 Quantity;
};

/** C++ FS_ItemUse */
USTRUCT(BlueprintType)
struct FS_ItemUse
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Usable Type", MakeStructureDefaultValue = "NewEnumerator0"))
	uint8 UsableType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Already Used"))
	bool AlreadyUsed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Remove Type", MakeStructureDefaultValue = "NewEnumerator0"))
	uint8 RemoveType;
};

/** C++ E_RuneType */
UENUM(BlueprintType)
enum class E_RuneType : uint8
{
	None,
	Ymir,
	Akash,
	Nuia,
	Hiram,
};

/** C++ E_RuneType */
UENUM(BlueprintType)
enum class E_UIType : uint8
{
	None,
	Character,
	Joural,
	Map,
	Ability,
	Crafting,
	Runes,
	CodeX,
	Glossary,
	Gallery,
	Control,
	Buy,
	Sell,
};

/** C++ FST_Stats */
USTRUCT(BlueprintType)
struct FST_Stats
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Stat"))
	E_StatType Stat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Value", MakeStructureDefaultValue = "0.000000"))
	double Value;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "HasCurrentValue", MakeStructureDefaultValue = "False"))
	bool HasCurrentValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "CurrentValue", MakeStructureDefaultValue = "0.000000"))
	double CurrentValue;

	/** If for example you increase the max HP then the current HP will also match the Max HP if this is set to true */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "OnValueChangedChangeCurrentValue", MakeStructureDefaultValue = "False"))
	bool OnValueChangedChangeCurrentValue;
};

/** C++ FST_Rune */
USTRUCT(BlueprintType)
struct FST_Rune
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "RuneID", MakeStructureDefaultValue = "None"))
	FName RuneID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Name"))
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Icon", MakeStructureDefaultValue = "None"))
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "RuneType"))
	E_RuneType RuneType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Stats"))
	TArray<FST_Stats> Stats;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Sellable?", MakeStructureDefaultValue = "False"))
	bool Sellable;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "SellValue", MakeStructureDefaultValue = "0.000000"))
	double SellValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "ShopIcon", MakeStructureDefaultValue = "None"))
	TObjectPtr<UTexture2D> ShopIcon;
};

/** C++ FS_RuneData */
USTRUCT(BlueprintType)
struct FS_RuneData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Rune Data"))
	FST_Rune RuneData;
};

/** C++ E_ArrowType */
UENUM(BlueprintType)
enum class E_ArrowType : uint8
{
	Basic,
	Ice,
	Poison,
	Explosion,
};

/** C++ E_ArrowType */
UENUM(BlueprintType)
enum class E_InTeractableType : uint8
{
	LootBox,
	Vendor,
	//Storage,
	////Forge,
};

/** C++ FS_RandomFloatValue */
USTRUCT(BlueprintType)
struct FS_RandomFloatValue
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Min Value"))
	float MinValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Max Value"))
	float MaxValue;

	/** 1111111111111111111111111 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Multiplier DataTable"))
	TObjectPtr<UDataTable> MultiplierDataTable;
};

/** C++ FS_ItemRandomStats */
USTRUCT(BlueprintType)
struct FS_ItemRandomStats
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Stat", MakeStructureDefaultValue = "NewEnumerator0"))
	E_StatType Stat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Value and Multiplier"))
	FS_RandomFloatValue ValueAndMultiplier;

	/** Rounds A to the nearest Integer */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Round to Int", MakeStructureDefaultValue = "False"))
	bool RoundToInt;
};

/** C++ FS_RandomIntegerValue */
USTRUCT(BlueprintType)
struct FS_RandomIntegerValue
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Min Value"))
	int32 MinValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Max Value"))
	int32 MaxValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Multiplier DataTable"))
	TObjectPtr<UDataTable> MultiplierDataTable;
};

/** C++ FS_ItemRandomLoot */
USTRUCT(BlueprintType)
struct FS_ItemRandomLoot
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Drop Percentage", MakeStructureDefaultValue = "0"))
	int32 DropPercentage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Min Quantity", MakeStructureDefaultValue = "0"))
	int32 MinQuantity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Max Quantity", MakeStructureDefaultValue = "0"))
	int32 MaxQuantity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Min Durability", MakeStructureDefaultValue = "0"))
	int32 MinDurability;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Max Durability", MakeStructureDefaultValue = "0"))
	int32 MaxDurability;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Random Stats"))
	TArray<FS_ItemRandomStats> RandomStats;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Random Value"))
	FS_RandomFloatValue RandomValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Random Required Level"))
	FS_RandomIntegerValue RandomRequiredLevel;
};

/** C++ FS_MountPhysicalPower */
USTRUCT(BlueprintType)
struct FS_MountPhysicalPower
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Sprint Stamina Cost", MakeStructureDefaultValue = "0.030000"))
	double SprintStaminaCost;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Stamina Regen Power", MakeStructureDefaultValue = "0.500000"))
	double StaminaRegenPower;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Charge Time", MakeStructureDefaultValue = "5.000000"))
	double ChargeTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Charge Consume", MakeStructureDefaultValue = "2.000000"))
	double ChargeConsume;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Turning Stamina Cost", MakeStructureDefaultValue = "6.000000"))
	double TurningStaminaCost;
};

/** C++ FS_MountPhysicalPower */
USTRUCT(BlueprintType)
struct FS_MountMobility
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Max Boost", MakeStructureDefaultValue = "4.000000"))
	double MaxBoost;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Dash Consume", MakeStructureDefaultValue = "1.000000"))
	double DashConsume;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Boost Regen Time", MakeStructureDefaultValue = "15.000000"))
	double BoostRegenTime;
};

/** C++ FS_MountData */
USTRUCT(BlueprintType)
struct FS_MountData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "Mount Class", MakeStructureDefaultValue = "None"))
	TObjectPtr<ACharacter> MountClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "MountType", MakeStructureDefaultValue = "0"))
	uint8 MountType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Mount Mobility"))
	FS_MountMobility MountMobility;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Mount Physical"))
	FS_MountPhysicalPower MountPhysical;
};

/** C++ FS_SingleDTItem */
USTRUCT(BlueprintType)
struct FS_SingleDTItem
{
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Table and Row"))
	FDataTableRowHandle TableAndRow;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Quantity"))
	int32 Quantity;
};

/** C++ E_DataTableType */
UENUM(BlueprintType)
enum class E_DataTableType : uint8
{
	E_ArmorDT,
	E_Cooking,
	E_Consumables,
	E_WeaponDT,
	E_Crafting_List,
	E_Crafting_IngredientDT,
	E_Ingredients_Fish,
	E_Gather_Resources,
	E_Glider,

};

/** C++ ItemData */
USTRUCT(BlueprintType)
struct FFS_ItemData : public FTableRowBase//,public UUserDefinedStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Type", MakeStructureDefaultValue = "NewEnumerator0"))
	E_ItemType Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Rarity", MakeStructureDefaultValue = "NewEnumerator7"))
	E_ItemRarity Rarity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Slot", MakeStructureDefaultValue = "NewEnumerator0"))
	E_ItemSlot Slot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Weapon Category", MakeStructureDefaultValue = "NewEnumerator0"))
	E_WeaponCategory WeaponCategory;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Inventory Panel", MakeStructureDefaultValue = "NewEnumerator10"))
	E_InventoryPanels InventoryPanel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Class", MakeStructureDefaultValue = "None"))
	TObjectPtr<UClass> Class;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Equippable Static Mesh", MakeStructureDefaultValue = "None"))
	TObjectPtr<UStaticMesh> EquippableStaticMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Equippable Skeletal Mesh", MakeStructureDefaultValue = "None"))
	TObjectPtr<USkeletalMesh> EquippableSkeletalMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Image", MakeStructureDefaultValue = "None"))
	TObjectPtr<UTexture2D> Image;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Index", MakeStructureDefaultValue = "0"))
	int32 Index;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Description"))
	FS_ItemDesc Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Stats"))
	FS_ItemStats Stats;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Stacks"))
	FS_ItemStacks Stacks;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Use"))
	FS_ItemUse Use;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Is Equipped", MakeStructureDefaultValue = "False"))
	bool IsEquipped;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Loot Randomizer"))
	FS_ItemRandomLoot LootRandomizer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Arrow Class"))
	TObjectPtr<UClass> ArrowClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Arrow Type Mesh", MakeStructureDefaultValue = "None"))
	TObjectPtr<UStaticMesh> ArrowTypeMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Quiver Type Mesh", MakeStructureDefaultValue = "None"))
	TObjectPtr<UStaticMesh> QuiverTypeMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Dual Wield Mesh", MakeStructureDefaultValue = "None"))
	TObjectPtr<UStaticMesh> DualWieldMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Arrow Effect", MakeStructureDefaultValue = "NewEnumerator0"))
	E_ArrowType ArrowEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Rune Data"))
	FS_RuneData RuneData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "ItemSetID"))
	FString ItemSetID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "ItemNameSet"))
	TArray<FText> ItemNameSet;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Mount Data"))
	FS_MountData MountData;
};

/** C++ FS_Crafting */
USTRUCT(BlueprintType)
struct FFS_Crafting : public FTableRowBase
{
	GENERATED_BODY()
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Craftable Item"))
	FS_SingleDTItem CraftableItem;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Required Items"))
	TArray<FS_SingleDTItem> RequiredItems;
};

DECLARE_DELEGATE_OneParam(FUpdateItem, const FFS_ItemData*);

