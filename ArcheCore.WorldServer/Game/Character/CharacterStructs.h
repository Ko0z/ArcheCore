#pragma once
#include "common_archecore.h"
#include "Entity/common_entity.h"
#include <array>
#include <vector>
#include <string>

enum class RelationState : uint8_t
{
	RELATIONSTATE_HOSTILE = 1,
	RELATIONSTATE_NEUTRAL = 2,
	RELATIONSTATE_FRIENDLY = 3
};

struct FactionRelation
{
	uint32_t id = 0;
	uint32_t id2 = 0;
	RelationState state;
	int64_t expiryTime = 0;
};

struct WorldSpawnPos {
	float X = 0;
	float Y = 0;
	float Z = 0;
	float Roll = 0;
	float Pitch = 0;
	float Yaw = 0;
	uint32_t WorldId = 1;
	uint32_t ZoneId = 0;
};

struct CharacterTemplateConfig {
	uint32_t Id;
	std::string Name;
	WorldSpawnPos WorldSpawnPos;
	uint8_t NumInventorySlot;
	uint16_t NumBankSlot;
};

struct CharacterTemplate {
	e::Race Race;
	e::Gender Gender;
	uint32_t ModelId;
	uint32_t ZoneId;
	uint32_t FactionId;
	uint32_t ReturnDistrictId;
	uint32_t ResurrectionDistrictId;
	WorldSpawnPos SpawnPosition;
	std::array<uint32_t, 7> Items = {};
	std::vector<uint32_t> Buffs = std::vector<uint32_t>(16);
	uint8_t NumInventorySlot;
	uint16_t NumBankSlot;
};

struct EquipItemsTemplate {
	uint32_t Headgear;
	uint8_t HeadgearGrade;
	uint32_t Necklace;
	uint8_t NecklaceGrade;
	uint32_t Shirt;
	uint8_t ShirtGrade;
	uint32_t Belt;
	uint8_t BeltGrade;
	uint32_t Pants;
	uint8_t PantsGrade;
	uint32_t Gloves;
	uint8_t GlovesGrade;
	uint32_t Shoes;
	uint8_t ShoesGrade;
	uint32_t Bracelet;
	uint8_t BraceletGrade;
	uint32_t Back;
	uint8_t BackGrade;
	uint32_t Cosplay;
	uint8_t CosplayGrade;
	uint32_t Undershirt;
	uint8_t UndershirtGrade;
	uint32_t Underpants;
	uint8_t UnderpantsGrade;
	uint32_t Mainhand;
	uint8_t MainhandGrade;
	uint32_t Offhand;
	uint8_t OffhandGrade;
	uint32_t Ranged;
	uint8_t RangedGrade;
	uint32_t Instrument;
	uint8_t InstrumentGrade;
};

struct AbilitySupplyItem {
	uint32_t Id = 0;
	int32_t Amount = 0;
	uint8_t Grade = 0;
};

struct AbilityItems {
	uint8_t Ability;
	EquipItemsTemplate Items;
	std::vector<AbilitySupplyItem> Supplies;
};

struct Expand {
	bool IsBank;
	int32_t Step;
	int32_t Price;
	uint32_t ItemId;
	int32_t ItemCount;
	int32_t CurrencyId;
};

struct AppellationTemplate {
	uint32_t Id;
	uint32_t BuffId;
};

struct ActabilityTemplate {
	uint32_t Id;
	std::string Name;
	int32_t UnitAttributeId;
};

struct ExpertLimit {
	uint32_t Id;
	int32_t UpLimit;
	uint8_t ExpertLimitCount;
	int32_t Advantage;
	int32_t CastAdvantage;
	uint32_t UpCurrencyId;
	int32_t UpPrice;
	uint32_t DownCurrecyId;
	int32_t DownPrice;
};

struct ExpandExpertLimit {
	uint32_t Id;
	uint8_t ExpandCount;
	int32_t LifePoint; // Vocation
	uint32_t ItemId;
	int32_t ItemCount;
};

