#pragma once
#include "common_archecore.h"

namespace e {

	enum BaseUnitType : uint8_t
	{
		CHARACTER = 0,
		NPC = 1,
		SLAVE = 2,
		HOUSING = 3,
		TRANSFER = 4,
		MATE = 5,
		SHIPYARD = 6
	};

	enum TypeMask : uint8_t			// Bit Mask
	{
		TYPEMASK_CHARACTER = 0x01,	// 00000001
		TYPEMASK_NPC = 0x02,		// 00000010
		TYPEMASK_SLAVE = 0x04,		// 00000100
		TYPEMASK_HOUSING = 0x08,	// 00001000
		TYPEMASK_TRANSFER = 0x10,	// 00010000
		TYPEMASK_MATE = 0x20,		// 00100000
		TYPEMASK_SHIPYARD = 0x40	// 01000000
	};

	enum class ModelPostureType : uint8_t
	{
		NONE = 0,
		HOUSESTATE = 1,
		ACTORMODELSTATE = 4,
		FARMFIELDSTATE = 7,
		TURRETSTATE = 8
	};

	enum class Race : uint8_t {
		None = 0,
		Nuian = 1,
		Fairy = 2,
		Dwarf = 3,
		Elf = 4,
		Hariharan = 5,
		Ferre = 6,
		Returned = 7,
		Warborn = 8
	};

	enum class Gender : uint8_t {
		Male = 1,
		Female = 2
	};

	enum class EquipmentItemSlot : uint8
	{
		Head = 0,
		Neck = 1,
		Chest = 2,
		Waist = 3,
		Legs = 4,
		Hands = 5,
		Feet = 6,
		Arms = 7,
		Back = 8,
		Ear1 = 9,
		Ear2 = 10,
		Finger1 = 11,
		Finger2 = 12,
		Undershirt = 13,
		Underpants = 14,
		Mainhand = 15,
		Offhand = 16,
		Ranged = 17,
		Musical = 18,
		Face = 19,
		Hair = 20,
		Glasses = 21,
		Reserved = 22,
		Tail = 23,
		Body = 24,
		Beard = 25,
		Backpack = 26,
		Cosplay = 27
	};

	enum class ItemDetailType : uint8
	{
		None = 0,
		Equipment = 1,
		Slave = 2,
		Mate = 3,
		Ucc = 4,
		Treasure = 5,
		BigFish = 6,
		Decoration = 7,
		MusicSheet = 8,
		Glider = 9,
		SlaveEquipment = 10,
		Location = 11,
		TypeMax = 12,
	};

	enum class ItemClassType : uint8 {
		ITEM = 0,
		EQUIP = 1,
		BACKPACK = 2,
		BODYPART = 3,
		MUSICSHEET = 4,
		RUNE = 5,
		SUMMON = 6,
		UCC = 7,
		ARMOR = 8,
		WEAPON = 9,
		ACCESSORY = 10
	};

	enum class SkillType : uint8
	{
		Skill = 1,
		Buff = 2
	};

	enum class SkillHitType : uint8
	{
		Invalid = 0x0,
		MeleeHit = 0x1,
		MeleeCritical = 0x3,
		MeleeMiss = 0x4,
		MeleeDodge = 0x5,
		MeleeBlock = 0x6,
		MeleeParry = 0x7,
		RangedHit = 0x9,
		RangedMiss = 0xA,
		RangedCritical = 0xB,
		SpellHit = 0xD,
		SpellMiss = 0xE,
		SpellCritical = 0xF,
		RangedDodge = 0x10,
		RangedBlock = 0x11,
		Immune = 0x12,
		SpellResist = 0x13,
		RangedParry = 0x14,
	};

	// Desribes what container type the item is in...
	enum class SlotType : uint8 {
		None = 0,
		Equipment = 1,
		Inventory = 2,
		Bank = 3,
		Trade = 4,
		Mail = 5,
		System = 0xFF
	};

	enum class AbilityType : uint8_t {
		General = 0,
		Fight = 1,
		Illusion = 2,
		Adamant = 3,
		Will = 4,
		Death = 5,
		Wild = 6,
		Magic = 7,
		Vocation = 8,
		Romance = 9,
		Love = 10,
		None = 11
	};

	enum class ActionSlotType : uint8 {
		None = 0,
		ItemType = 1,
		Spell = 2,
		Macro = 3,
		ItemId = 4,
		RidePetSpell = 5
	};

	enum class ItemBindType : uint8
	{
		Normal = 1,
		BindOnPickup = 2,
		BindOnEquip = 3,
		BindOnUnpack = 4,
		BindOnPickupPack = 5,
		BindOnAuctionWin = 6,
	};

	enum class ItemFlag : uint8
	{
		None = 0x00,
		SoulBound = 0x01,
		HasUCC = 0x02,
		Secure = 0x04,
		Skinized = 0x08,
		Unpacked = 0x10,
		AuctionWin = 0x20
	};

	inline ItemFlag operator| (ItemFlag a, ItemFlag b) {
		return static_cast<ItemFlag> (static_cast<uint8>(a) | static_cast<uint8>(b));
	}

	enum class ItemTaskType : uint8
	{
		Invalid = 0,
		Destroy = 1,
		AboxDestroy = 2,
		Repair = 3,
		DurabilityLoss = 4,
		SwapItems = 5,
		Split = 6,
		SplitCofferItems = 7,
		SwapCofferItems = 8,
		Loot = 9,
		LootAll = 10,
		Gm = 11,
		GameRuleReset = 12,
		ConsumeSkillSource = 13, // 303-consume-skill-source
		DoodadCreate = 14, // 303-doodad-create
		DoodadRemove = 15,
		DoodadItemChanger = 16,
		DoodadInteraction = 17,
		DoodadCattleFeed = 18,
		UpgradeSkill = 19,
		AbilityChange = 20,
		AbilityReset = 21,
		BuyPriestBuff = 22,
		Teleport = 23,
		CapturePet = 24,
		RecoverDoodadItem = 25, // 508-recover-doodad-item
		MateCreate = 26,
		CraftActSaved = 27,
		CraftPaySaved = 28,
		CraftPickupProduct = 29,
		CraftCancel = 30,
		HouseCreation = 31, // 303-house-creation
		HouseDeposit = 32,
		HouseBuilding = 33,
		PickupBloodstain = 34,
		AutoLootDoodadItem = 35, // 316-autoloot-doodad-item
		QuestStart = 36, // 401-quest-start
		QuestComplete = 37, // 402-quest-complete
		QuestSupplyItems = 38, // 405-quest-supply-items
		QuestRemoveSupplies = 39, // 402-403-404-405-quest-remove-supplies
		SkillReagents = 40,
		SkillEffectConsumption = 41,
		SkillEffectGainItem = 42,
		SkillEffectGainItemWithPos = 43,
		SkillEffectSiegeTicket = 44,
		SkillEffectExpToItem = 45,
		Auction = 46,
		Mail = 47,
		Trade = 48,
		EnchantMagical = 49,
		EnchantPhysical = 50,
		GetCoinByItem = 51,
		GetItemByCoin = 52,
		StoreSell = 53, // 315-store-sell
		StoreBuy = 54, // 313-314-store-buy
		TodReward = 55,
		GainItemWithUcc = 56,
		ImprintUcc = 57,
		RepairPets = 58,
		MateDeath = 59,
		Shipyard = 60, // 303-shipyard
		SkillsReset = 61,
		DropBackpack = 62,
		UseRelic = 63,
		UseIndependenceRelic = 64,
		Conversion = 65, // 304-conversion
		Seize = 66,
		ReturnSeized = 67,
		DemoDressOff = 68,
		DemoDressOn = 69,
		DemoClearBag = 70,
		DemoFillBag = 71,
		SlaveDeath = 72,
		ExpeditionCreation = 73,
		RepairSlaves = 74,
		ExpandBag = 75,
		ExpandBank = 76,
		RenewEquipment = 77,
		LifespanExpiration = 78,
		RecoverExp = 79,
		SpawnerUpdate = 80,
		UpdateSummonSlaveItem = 81,
		UpdateSummonMateItem = 82,
		DepositMoney = 83,
		WithdrawMoney = 84,
		DeliverItemToOthers = 85,
		SetSlavePosition = 86,
		SetBountyMoney = 87,
		PayBountyMoney = 88,
		ConvertFish = 89,
		Fishing = 90,
		SellHouse = 91,
		BuyHouse = 92,
		SaveMusicNotes = 93,
		ItemLock = 94,
		ItemUnlock = 95,
		ItemUnlockExcess = 96,
		GradeEnchant = 97,
		RechargeBuff = 98,
		Socketing = 99,
		Dyeing = 100,
		ConsumeIndunTicket = 101,
		ExpandExpert = 102,
		Exchange = 103,
		SellBackpack = 104,
		SellSpecialty = 105,
		AskMould = 106,
		TakeMould = 107,
		FactionDeclareHostile = 108,
		EditCosmetic = 109,
		ChangeAutoUseAaPoint = 110,
		ConvertItemLook = 111,
		ChangeExpertLimit = 112,
		Sknize = 113,
		ItemTaskThistimeUnpack = 114,
		BuyPremiumService = 115,
		BuyAaPoint = 116
	};

	enum class AttachPointKind : uint8
	{
		None = 0,
		Driver = 1,
		Passenger0 = 2,
		Passenger1 = 3,
		Passenger2 = 4,
		Passenger3 = 5,
		Passenger4 = 6,
		Passenger5 = 7,
		Passenger6 = 8,
		Cannon0 = 9,
		Cannon1 = 10,
		Cannon2 = 11,
		Cannon3 = 12,
		Cannon4 = 13,
		Cannon5 = 14,
		Cannon6 = 15,
		Cannon7 = 16,
		Cannon8 = 17,
		Hook = 18,
		Mast0 = 19,
		Mast1 = 20,
		Sail0 = 21,
		Sail1 = 22,
		Anchor = 23,
		LadderLeft = 24,
		LadderRight = 25,
		PlankLeft = 26,
		PlankRight = 27,
		LampFront = 28,
		LampRear = 29,
		Trailed0 = 30,
		Trailed1 = 31,
		Trailed2 = 32,
		Trailed3 = 33,
		Trailed4 = 34,
		Helms = 35,
		HealPoint0 = 36,
		HealPoint1 = 37,
		HealPoint2 = 38,
		HealPoint3 = 39,
		HealPoint4 = 40,
		HealPoint5 = 41,
		HealPoint6 = 42,
		HealPoint7 = 43,
		HealPoint8 = 44,
		HealPoint9 = 45,
		Cannon9 = 46,
		Cannon10 = 47,
		Cannon11 = 48,
		Cannon12 = 49,
		Cannon13 = 50,
		Cannon14 = 51,
		Cannon15 = 52,
		Cannon16 = 53,
		Cannon17 = 54,
		Cannon18 = 55,
		Cannon19 = 56,
		NamePlate01 = 57,
		LadderRearLeft = 58,
		LadderRearRight = 59,
		Box0 = 60,
		Box1 = 61,
		Box2 = 62,
		Box3 = 63,
		Box4 = 64,
		Box5 = 65,
		Box6 = 66,
		Box7 = 67,
		Box8 = 68,
		Box9 = 69,
		Box10 = 70,
		Box11 = 71,
		Box12 = 72,
		Box13 = 73,
		Box14 = 74,
		Box15 = 75,
		Box16 = 76,
		Box17 = 77,
		Box18 = 78,
		Box19 = 79,
		Telescope = 80,
		Bell = 81,
		Rudder = 82,
		Sail2 = 83,
		Mast2 = 84,
		System = 255
	};
}