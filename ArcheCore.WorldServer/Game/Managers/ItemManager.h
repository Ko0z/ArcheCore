#pragma once
#include "common_archecore.h"
#include "Entity/common_entity.h"
#include "Items/ItemDefines.h"
#include <unordered_map>

class ItemManager {
private:
	ItemManager() {}

public:
	static ItemManager& Instance() {
		static ItemManager _Instance;
		return _Instance;
	}

	//Item* CreateItem(uint32 _templateId, uint32 count, uint8 grade);
	void AquireDefaultItem(e::CharacterInventory& inv, e::ItemTaskType taskType, uint32 itemId, int32 amount = 1, int32 grade = -1, uint32 crafterId = 0);

	entt::entity CreateItem(entt::registry& world, uint32 itemId, uint32 count, uint8 grade);

	const ItemTemplate* GetItemTemplate(uint32 id);

	void Initialize();

private:
	bool LoadItemDatabase();

private:
	bool isInitialized = false;

	// TODO remove TEMP item ID generator
	uint64 m_NextItemId = 4000;

	ItemConfig m_ItemConfig;

	// TODO: Handle all created Objects in a central ObjectManager for GUID's etc...
	// Keep all created items here, everywhere else only ref
	//std::unordered_map<uint64, std::shared_ptr<Item>> m_AllItems;

	// All this is TEMP and should be fetched at runtime and at the
	// time of need for specific data. Storing all this in memory 
	// wont allow to recognize changes made to the database during uptime.

	// Has to be ItemTemplate* otherwise the inherited stuff is lost?
	std::unordered_map<uint32, ItemTemplate*> m_ItemTemplates;

	std::unordered_map<uint32, Holdable> m_Holdables;
	std::unordered_map<uint32, Wearable> m_Wearables;
	std::unordered_map<uint32, WearableKind> m_WearableKinds;
	std::unordered_map<uint32, WearableSlot> m_WearableSlots;
	std::unordered_map<uint32, AttributeModifier> m_AttributeModifiers;

	// Grade Enchanting
	std::unordered_map<uint32, EquipSlotEnchantingCost> m_EnchantingCosts;
	std::unordered_map<uint32, GradeTemplate> m_GradeTemplates;
	std::unordered_map<uint32, ItemGradeEnchantingSupport> m_EnchantingSupports;

	// Gemming
	std::unordered_map<uint32, uint32> m_SocketChance;
	std::unordered_map<uint32, ItemCapScale> m_ItemCapScales;

	// ItemLookConvert
	std::unordered_map<uint32, ItemLookConvert> m_ItemLookConverts;
	std::unordered_map<uint32, uint32> m_HoldableItemLookConverts;
	std::unordered_map<uint32, uint32> m_WearableItemLookConverts;

	std::unordered_map<uint32, ItemProcTemplate> m_ItemProcTemplates;
	std::unordered_map<uint32, EquipItemSet> m_EquipItemSets;
};