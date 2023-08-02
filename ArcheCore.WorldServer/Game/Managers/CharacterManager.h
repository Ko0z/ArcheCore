#pragma once

#include "common_archecore.h"
#include "common_utils.h"
#include "Character/CharacterStructs.h"
#include "Items/ItemDefines.h"
#include "Entity/common_entity.h"
#include "Entity/UnitCustomModelData.h"

#include <unordered_set>
#include <unordered_map>

//class UnitCustomModelData;
class WorldSession;
//class Character;

/// <summary>
/// CharacterManager is a singleton class used when creating character etc..
/// </summary>
class CharacterManager
{
private:
	CharacterManager() {}

public:
	static CharacterManager& Instance() {
		static CharacterManager _Instance;
		return _Instance;
	}

	void Load();

	void Create(WorldSession& worldSession, std::string& name, uint8_t race, uint8_t gender, uint32_t body[],
		e::UnitCustomModelData& customModel, uint8_t ability1);

	uint8_t ValidateCharacterName(std::string& name);

	const CharacterTemplate& GetTemplate(uint8_t race, uint8_t gender)
	{
		return m_Templates[(uint8_t)(16 * gender + race)];
	}

private:
	
	//void SetEquipItemTemplate(std::shared_ptr<Inventory> inventory, uint32 templateId, EquipmentItemSlot slot, uint8 grade);

	void CreateDefaultItemSlot(entt::registry& world, e::InventoryEquipped& invEquip, uint32 templateId, e::EquipmentItemSlot slot, uint8 grade);

	void HandleCharEnumCallback()
	{

	}

	void HandlePlayerLoginCallback()
	{
		// 
	}

	//TEMP
	uint64 m_NextCharacterId = 1;
	uint32 m_NextObjectId = 256;

	std::unordered_map<uint8_t, CharacterTemplate> m_Templates;
	std::unordered_map<uint8_t, AbilityItems> m_AbilityItems;
	std::unordered_map<int32_t, std::vector<Expand>> m_Expands;
	std::unordered_map<uint32_t, AppellationTemplate> m_Appellations;
	std::unordered_map<uint32_t, ActabilityTemplate> m_Actabilites;
	std::unordered_map<int32_t, ExpertLimit> m_ExpertLimits;
	std::unordered_map<int32_t, ExpandExpertLimit> m_ExpandExpertLimits;
	
};