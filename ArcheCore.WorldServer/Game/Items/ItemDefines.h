#pragma once
#include <cstdint>
#include <Character/CharacterStructs.h>
#include <vector>
#include "Entity/Items.h"

constexpr uint32_t COINS = 500;
constexpr uint32_t TAX_CERTIFICATE = 31891;
constexpr uint32_t BOUND_TAX_CERTIFICATE = 31892;
constexpr uint32_t APPRAISAL_CERTIFICATE = 28085;
constexpr uint32_t CREST_STAMP = 17662;
constexpr uint32_t CREST_INK = 17663;
constexpr uint32_t SHEET_MUSIC = 28051;
constexpr uint32_t SALON_CERTIFICATE = 30811;

struct ItemCapScale {
	uint32 Id;
	uint32 SkillId;
	int32 ScaleMin;
	int32 ScaleMax;
};

struct ItemGradeEnchantingSupport {
	uint32 Id;
	uint32 ItemId;
	int32 RequireGradeMax;
	int32 RequireGradeMin;
	int32 AddSuccessRatio;
	int32 AddSuccessMul;
	int32 AddGreatSuccessRatio;
	int32 AddGreatSuccessMul;
	int32 AddBreakRatio;
	int32 AddBreakMul;
	int32 AddDowngradeRatio;
	int32 AddDowngradeMul;
	int32 AddGreatSuccessGrade;
};

struct EquipSlotEnchantingCost {
	uint32 Id;
	uint32 SlotTypeId;
	int32 Cost;
};

struct EquipItemSetBonus {
	int32 NumPieces;
	uint32 BuffId;
	uint32 ItemProcId;
};

struct EquipItemSet {
	uint32 Id;
	std::vector<EquipItemSetBonus> Bonuses;
};

enum class BackpackType : uint8 {
	CastleClaim = 1,
	Glider = 2,
	TradePack = 3,
	SiegeDeclare = 4,
	NationFlag = 5,
	Fish = 6,
	ToyFlag = 7
};

enum class ProcChanceKind : uint8
{
	HitAny = 1,
	HitMelee = 2,
	HitMeleeCrit = 3,
	HitSpell = 4,
	HitSpellCrit = 5,
	HitRanged = 6,
	HitRangedCrit = 7,
	HitSiege = 8,
	TakeDamageAny = 9,
	TakeDamageMelee = 10,
	TakeDamageMeleeCrit = 11,
	TakeDamageSpell = 12,
	TakeDamageSpellCrit = 13,
	TakeDamageRanged = 14,
	TakeDamageRangedCrit = 15,
	TakeDamageSiege = 16,
	HitHeal = 17,
	HitHealCrit = 18,
	FireSkill = 19,
	HitSkill = 20,
};

struct ItemProcTemplate {
	uint32 Id;
	uint32 SkillId;
	ProcChanceKind ChanceKindId;
	uint32 ChanceRate;
	uint32 ChanceParam; // Always zero in 1.2
	uint32 CooldownSec;
	bool Finisher;
	uint32 ItemLevelBasedChanceBonus;

	//SkillTemplate SkillTemplate;
};

struct AttributeModifier {
	uint32 Id;
	int32 StrWeight;
	int32 DexWeight;
	int32 StaWeight;
	int32 IntWeight;
	int32 SpiWeight;

	int32 AllWeight() { return StrWeight + DexWeight + StaWeight + IntWeight + SpiWeight; }

	int32 Count() {
		int32 result = 0;

		if (StrWeight > 0) { result++; }
		if (DexWeight > 0) { result++; }
		if (StaWeight > 0) { result++; }
		if (IntWeight > 0) { result++; }
		if (SpiWeight > 0) { result++; }

		return result;
	}
};

struct WearableSlot {
	uint32 SlotTypeId;
	int32 Coverage;
};

struct WearableKind {
	uint32 TypeId;
	int32 ArmorRatio;
	int32 MagicResistanceRatio;
	uint32 FullBufId;
	uint32 HalfBufId;
	int32 ExtraDamagePierce;
	int32 ExtraDamageSlash;
	int32 ExtraDamageBlunt;
	float DurabilityRatio;
};

struct Wearable {
	uint32 TypeId;
	uint32 SlotTypeId;
	int32 ArmorBp;
	int32 MagicResistanceBp;
};

struct Holdable {
	uint32 Id;
	uint32 KindId;
	int32 Speed;
	int32 ExtraDamagePierceFactor;
	int32 ExtraDamageSlashFactor;
	int32 ExtraDamageBluntFactor;
	int32 MaxRange;
	int32 Angle;
	int32 EnchantedDps1000;
	uint32 SlotTypeId;
	int32 DamageScale;
	
	//Formula FormulaDps;
	//Formula FormulaMDps;
	//Formula FormulaArmor;
	//Formula FormulaHDps;
	int32 MinRange;
	int32 SheathePriority;
	float DurabilityRatio;
	int32 RenewCategory;
	int32 ItemProcId;
	int32 StatMultiplier;
};

struct GradeTemplate {
	int32 Grade;
	//int32 GradeOrder;
	float HoldableDps;
	float HoldableArmor;
	float HoldableMagicDps;
	float HoldableHealDps;
	float WearableArmor;
	float WearableMagicResistance;
	float Durability;
	int32 UpgradeRatio;
	int32 StatMultiplier;
	int32 RefundMultiplier;
	int32 EnchantSuccessRatio;
	int32 EnchantGreatSuccessRatio;
	int32 EnchantBreakRatio;
	int32 EnchantDowngradeRatio;
	int32 EnchantDowngradeMin;
	int32 EnchantDowngradeMax;
	int32 EnchantCost;
	int32 CurrencyId;
};

struct ItemConfig {
	float DurabilityDecrementChance;
	float DurabilityRepairCostFactor;
	float DurabilityConst;
	float HoldableDurabilityConst;
	float WearableDurabilityConst;
	int32 DeathDurabilityLossRatio;
	int32 ItemStatConst;
	int32 HoldableStatConst;
	int32 WearableStatConst;
	int32 StatValueConst;
};

enum class ItemTypeMask : uint32 {
	Invalid					= 0b00000000000000000000000000000000,
	Misc					= 0b00000000000000000000000000000001, // 0000 0000 0000 0000 0000 0000 0000 0001 - 0x00000001
	Weapon					= 0b00000000000000000000000000000010, // 0000 0000 0000 0000 0000 0000 0000 0010 - 0x00000002
	Armor					= 0b00000000000000000000000000000100, // 0000 0000 0000 0000 0000 0000 0000 0100 - 0x00000004
	Body					= 0b00000000000000000000000000001000, // 0000 0000 0000 0000 0000 0000 0000 1000 - 0x00000008
	Bag						= 0b00000000000000000000000000010000, // 0000 0000 0000 0000 0000 0000 0001 0000 - 0x00000010
	Housing					= 0b00000000000000000000000000100000, // 0000 0000 0000 0000 0000 0000 0010 0000 - 0x00000020
	HousingDecoration		= 0b00000000000000000000000001000000, // 0000 0000 0000 0000 0000 0000 0100 0000 - 0x00000040
	Tool					= 0b00000000000000000000000010000000, // 0000 0000 0000 0000 0000 0000 1000 0000 - 0x00000080
	SummonSlave				= 0b00000000000000000000000100000000, // 0000 0000 0000 0000 0000 0001 0000 0000 - 0x00000100
	SpawnDoodad				= 0b00000000000000000000001000000000, // 0000 0000 0000 0000 0000 0010 0000 0000 - 0x00000200
	AcceptQuest				= 0b00000000000000000000010000000000, // 0000 0000 0000 0000 0000 0100 0000 0000 - 0x00000400
	SummonMate				= 0b00000000000000000000100000000000, // 0000 0000 0000 0000 0000 1000 0000 0000 - 0x00000800
	Recipe					= 0b00000000000000000001000000000000, // 0000 0000 0000 0000 0001 0000 0000 0000 - 0x00001000
	Crafting				= 0b00000000000000000010000000000000, // 0000 0000 0000 0000 0010 0000 0000 0000 - 0x00002000
	Portal					= 0b00000000000000000100000000000000, // 0000 0000 0000 0000 0100 0000 0000 0000 - 0x00004000
	EnchantingGem			= 0b00000000000000001000000000000000, // 0000 0000 0000 0000 1000 0000 0000 0000 - 0x00008000
	ReportCrime				= 0b00000000000000010000000000000000, // 0000 0000 0000 0001 0000 0000 0000 0000 - 0x00010000
	LogicDoodad				= 0b00000000000000100000000000000000, // 0000 0000 0000 0010 0000 0000 0000 0000 - 0x00020000
	HasUCC					= 0b00000000000001000000000000000000, // 0000 0000 0000 0100 0000 0000 0000 0000 - 0x00040000
	OpenEmblemUi			= 0b00000000000010000000000000000000, // 0000 0000 0000 1000 0000 0000 0000 0000 - 0x00080000
	Shipyard				= 0b00000000000100000000000000000000, // 0000 0000 0001 0000 0000 0000 0000 0000 - 0x00100000
	Socket					= 0b00000000001000000000000000000000, // 0000 0000 0010 0000 0000 0000 0000 0000 - 0x00200000
	Backpack				= 0b00000000010000000000000000000000, // 0000 0000 0100 0000 0000 0000 0000 0000 - 0x00400000
	OpenPaper				= 0b00000000100000000000000000000000, // 0000 0000 1000 0000 0000 0000 0000 0000 - 0x00800000
	Accessory				= 0b00000001000000000000000000000000, // 0000 0001 0000 0000 0000 0000 0000 0000 - 0x01000000
	Treasure				= 0b00000010000000000000000000000000, // 0000 0010 0000 0000 0000 0000 0000 0000 - 0x02000000
	MusicSheet				= 0b00000100000000000000000000000000, // 0000 0100 0000 0000 0000 0000 0000 0000 - 0x04000000
	Dyeing					= 0b00001000000000000000000000000000, // 0000 1000 0000 0000 0000 0000 0000 0000 - 0x08000000
	SlaveEquipment			= 0b00010000000000000000000000000000, // 0001 0000 0000 0000 0000 0000 0000 0000 - 0x10000000
	GradeEnchantingSupport	= 0b00100000000000000000000000000000, // 0010 0000 0000 0000 0000 0000 0000 0000 - 0x20000000
	MateArmor				= 0b01000000000000000000000000000000, // 0100 0000 0000 0000 0000 0000 0000 0000 - 0x40000000
	Location				= 0b10000000000000000000000000000000  // 1000 0000 0000 0000 0000 0000 0000 0000 - 0x80000000
};

// If you add another field here, make sure to also handle it respectively when
// creating a new Item in the ItemManager::CreateItem
//enum class ItemClassType : uint8 {
//	ITEM = 0,
//	EQUIP = 1,
//	BACKPACK = 2,
//	BODYPART = 3,
//	MUSICSHEET = 4,
//	RUNE = 5,
//	SUMMON = 6,
//	UCC = 7,
//	ARMOR = 8,
//	WEAPON = 9,
//	ACCESSORY = 10
//};

// ItemImplEnum
enum class ItemTypeId : uint8
{
	Misc = 0,
	Weapon = 1,
	Armor = 2,
	Body = 3,
	Bag = 4,
	Housing = 5,
	HousingDecoration = 6,
	Tool = 7,
	SummonSlave = 8,
	SpawnDoodad = 9,
	AcceptQuest = 10,
	SummonMate = 11,
	Recipe = 12,
	Crafting = 13,
	Portal = 14,
	EnchantingGem = 15,
	ReportCrime = 16,
	LogicDoodad = 17,
	HasUCC = 18,
	OpenEmblemUi = 19,
	Shipyard = 20,
	Socket = 21,
	Backpack = 22,
	OpenPaper = 23,
	Accessory = 24,
	Treasure = 25,
	MusicSheet = 26,
	Dyeing = 27,
	SlaveEquipment = 28,
	GradeEnchantingSupport = 29,
	MateArmor = 30,
	Location = 31,
	Invalid = 32
};

//enum class ItemBindType : uint8
//{
//	Normal = 1,
//	BindOnPickup = 2,
//	BindOnEquip = 3,
//	BindOnUnpack = 4,
//	BindOnPickupPack = 5,
//	BindOnAuctionWin = 6,
//};

struct ItemLookConvert {
	uint32 Id;
	//uint32 SlotId;
	uint32 RequiredItemId;
	int32 RequiredItemCount;
};

//enum class ItemDetailType : uint8
//{
//	Invalid = 0,
//	Equipment = 1,
//	Slave = 2,
//	Mate = 3,
//	Ucc = 4,
//	Treasure = 5,
//	BigFish = 6,
//	Decoration = 7,
//	MusicSheet = 8,
//	Glider = 9,
//	SlaveEquipment = 10,
//	Location = 11,
//	TypeMax = 12,
//};


/*
enum class EquipmentItemSlotType : uint8
{
	Head = 1,
	Neck = 2,
	Chest = 3,
	Waist = 4,
	Legs = 5,
	Hands = 6,
	Feet = 7,
	Arms = 8,
	Back = 9,
	Ear = 10,
	Finger = 11,
	Undershirt = 12,
	Underpants = 13,
	Mainhand = 14,
	Offhand = 15,
	TwoHanded = 16,
	OneHanded = 17,
	Ranged = 18,
	Ammunition = 19,
	Shield = 20,
	Instrument = 21,
	Bag = 22,
	Face = 23,
	Hair = 24,
	Glasses = 25,
	Reserved = 26,
	Tail = 27,
	Body = 28,
	Beard = 29,
	Backpack = 30,
	Cosplay = 31
};
*/
/*
enum class ItemFlag : uint8_t
{
	None = 0x00,
	SoulBound = 0x01,
	HasUCC = 0x02,
	Secure = 0x04,
	Skinized = 0x08,
	Unpacked = 0x10,
	AuctionWin = 0x20
};
*/

enum class ShopCurrencyType : uint8_t
{
	Money = 0,
	Honor = 1,
	VocationBadges = 2,
	SiegeShop = 3,
};

struct ItemLocation
{
	e::SlotType SlotType;
	uint8_t Slot;
};

struct ItemIdAndLocation
{
	uint64_t Id;
	e::SlotType SlotType;
	uint8_t Slot;
};

struct ItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::ITEM; }
	
	uint32 Id;			// The entry id of all items in database
	std::string Name;
	int32 CategoryId;
	int32 Level;
	int32 Price;
	int32 Refund;
	e::ItemBindType BindType;
	int32 PickupLimit;
	int32 MaxCount;
	bool Sellable;
	uint32 UseSkillId;
	bool UseSkillAsReagent;
	ItemTypeId ImplId;			// Very specific item type
	e::ItemDetailType DetailType;	// General item type
	uint32 BuffId;		
	bool Gradeable;
	bool LootMulti;
	uint32 LootQuestId;
	int32 HonorPrice;
	int32 ExpAbsLifetime;
	int32 ExpOnlineLifetime;
	time_t ExpDate;
	int32 LevelReq;
	int32 AuctionCategoryA;
	int32 AuctionCategoryB;
	int32 AuctionCategoryC;
	int32 LevelLimit;
	int32 FixedGrade;
	bool Disenchantable;
	int32 LivingPointPrice;
	uint8 CharGender;
};

struct BodyPartTemplate : ItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::BODYPART; }

	uint32 ModelId;
	bool NpcOnly;
	bool BeautyShopOnly;
	uint32 ItemId;
	uint32 SlotTypeId;
};

struct MusicSheetTemlate : ItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::MUSICSHEET; }
};

struct RuneTemplate : ItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::RUNE; }

	uint32 EquipSlotGroupId;
	uint8 EquipLevel;
	uint8 ItemGradeId;
};

struct SummonMateTemplate : ItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::SUMMON; }

	uint32 NpcId;
};

struct UccTemplate : ItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::UCC; }
};

// Not sure about this one...
struct SummonSlaveTemplate : ItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::SUMMON; }

	uint32 SlaveId;
};

struct BackpackTemplate : ItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::BACKPACK; }
	
	uint32 AssetId;
	BackpackType BackpackType;
	uint32 DeclareSiegeZoneGroupId;
	bool Heavy;
	uint32 Asset2Id;
	bool NormalSpeciality;
	bool UseAsStat;
	uint32 SkinKindId;
};

struct EquipItemTemplate : public ItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::EQUIP; }
	
	uint32 ModSetId;
	bool Repairable;
	int32 DurabilityMultiplier;
	uint32 RechargeBuffId;
	int32 ChargeLifetime;
	int32 ChargeCount;
	ItemLookConvert* ItemLookConvert;
	uint32 EquipItemSetId;
};

struct ArmorTemplate : public EquipItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::ARMOR; }
	
	Wearable* WearableTemplate;
	WearableKind* KindTemplate;
	WearableSlot* SlotTemplate;
	bool BaseEnchantable;
	bool BaseEquipment;
};

struct WeaponTemplate : public EquipItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::WEAPON; }

	Holdable* HoldableTemplate;
	bool BaseEnchantable;
	bool BaseEquipment;
};

struct AccessoryTemplate : public EquipItemTemplate {
	virtual e::ItemClassType ClassType() const { return e::ItemClassType::ACCESSORY; }

	Wearable* WearableTemplate;
	WearableKind* KindTemplate;
	WearableSlot* SlotTemplate;
};

