#include "ItemManager.h"
#include "common_utils.h"
#include <iostream>
#include "Utility/SQLiteUtility.h"
#include "SqLiteManager.h"
#include "Managers/WorldManager.h"
#include <spdlog/spdlog.h>

using namespace e;

void ItemManager::AquireDefaultItem(e::CharacterInventory& inv, ItemTaskType taskType, uint32 itemId, int32 amount, int32 grade, uint32 crafterId)
{
	if (amount <= 0) { spdlog::error("ERROR trying to aquire default item."); return; }

	const ItemTemplate* itemTemplate = GetItemTemplate(itemId);
	if (!itemTemplate) {
		return;
	}

	entt::registry& world = WorldManager::Instance().GetDefaultWorld();

	entt::entity item = CreateItem(world, itemTemplate->Id, amount, grade);

	// TODO: Make accurate checks etc...
	if (world.valid(item)) {
		for (uint8 i = 0; i < inv.Items.size(); i++)
		{
			if (inv.Items[i] == entt::null) {
				// Free spot? lets do it
				inv.Items[i] = item;
				//spdlog::info("Added Default item: {} of type: {} slot: {}", itemTemplate->Id, (uint32)itemTemplate->ImplId, i);
				return;
			}
		}
	}

	return;
}

entt::entity ItemManager::CreateItem(entt::registry& world, uint32 itemId, uint32 count, uint8 grade)
{
	const ItemTemplate* itemTemplate = GetItemTemplate(itemId);

	if (!itemTemplate) {
		return entt::null;
	}

	entt::entity item = world.create();
	//spdlog::info("Creating item: {} of type: {}", itemTemplate->Id, (uint32)itemTemplate->ImplId);
	ItemSlot& itemSlot = world.emplace<ItemSlot>(item);
	itemSlot.Id = itemId;
	itemSlot.Guid = m_NextItemId++;
	itemSlot.Count = 1;
	itemSlot.ItemFlag = itemTemplate->BindType == ItemBindType::BindOnPickup ? ItemFlag::SoulBound : ItemFlag::None;
	itemSlot.Grade = itemTemplate->FixedGrade >= 0 ? (uint8)itemTemplate->FixedGrade : grade;
	
	ItemLifeSpanComp& lifeComp = world.emplace<ItemLifeSpanComp>(item);
	lifeComp.CreatedTime = archecore::utils::GetUnixTimeNowSigned();
	
	ItemClassType classType = itemTemplate->ClassType();

	if (classType == ItemClassType::ARMOR || classType == ItemClassType::WEAPON || classType == ItemClassType::ACCESSORY) {
		ItemEquipable& equipComp = world.emplace<ItemEquipable>(item);
		// todo, set maxdurability by template values...
		equipComp.Durability = 100;
		ItemGems& gems = world.emplace<ItemGems>(item);
		ItemEnchants& enchants = world.emplace<ItemEnchants>(item);
		itemSlot.SlotType = SlotType::Equipment;
		//itemSlot.Type = ItemClassType::EQUIP;
		itemSlot.DetailType = ItemDetailType::Equipment;

		if (classType == ItemClassType::ARMOR) {
			itemSlot.Type = ItemClassType::ARMOR;
			// Add Armor specific component
		}

		if (classType == ItemClassType::WEAPON) {
			itemSlot.Type = ItemClassType::WEAPON;
			// Add Weapon specific component
		}

		if (classType == ItemClassType::ACCESSORY) {
			itemSlot.Type = ItemClassType::ACCESSORY;
			// Add Accessory specific component
		}

		return item;
	}

	if (classType == ItemClassType::BODYPART) {
			world.emplace<ItemBodyPart>(item);
			itemSlot.Type = ItemClassType::BODYPART;

			return item;
	}
	
	return item;
}

// It kinda has to be pointer returned becuase of inherited cases...
const ItemTemplate* ItemManager::GetItemTemplate(uint32 id)
{
	if (m_ItemTemplates.contains(id)) {
		return m_ItemTemplates[id];
	}

	spdlog::error("[ItemManager][ERROR] - Tried to fetch invalid or missing template ID.");
	return nullptr;
}

void ItemManager::Initialize()
{
	if (isInitialized == true) {
		spdlog::error("[ItemManager][ERROR] - Tried to load item DB when already loaded!");
		return;
	}

	if (LoadItemDatabase()) {
		isInitialized = true;
		return;
	}

	spdlog::error("[ItemManager][ERROR] - Loading item database failed!!");
}

bool ItemManager::LoadItemDatabase()
{
	if (isInitialized == true) { return false; }

	SqLiteManager& sm = SqLiteManager::Instance();
	sqlite3* db = sm.SqlOpen();

	// ------------>
	//spdlog::info("[ItemManager] - Loading Item Templates...");

	const char* command = "SELECT * FROM item_configs";
	sqlite3_stmt* stmt = sm.SqlPrepare(db, command);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		
		m_ItemConfig.DurabilityDecrementChance = (float)sqlite3_column_double(stmt, 1);
		m_ItemConfig.DurabilityRepairCostFactor = (float)sqlite3_column_double(stmt, 2);
		m_ItemConfig.DurabilityConst = (float)sqlite3_column_double(stmt, 3);
		m_ItemConfig.HoldableDurabilityConst = (float)sqlite3_column_double(stmt, 4);
		m_ItemConfig.WearableDurabilityConst = (float)sqlite3_column_double(stmt, 5);

		m_ItemConfig.DeathDurabilityLossRatio = sqlite3_column_int(stmt, 6);
		m_ItemConfig.ItemStatConst = sqlite3_column_int(stmt, 7);
		m_ItemConfig.HoldableStatConst = sqlite3_column_int(stmt, 8);
		m_ItemConfig.WearableStatConst = sqlite3_column_int(stmt, 9);
		m_ItemConfig.StatValueConst = sqlite3_column_int(stmt, 10);
		
	} sqlite3_finalize(stmt);

	// DEBUG, was correct
	//std::cout << std::endl << "[ItemManager][DEBUG] - "
	//	<< m_ItemConfig.DurabilityDecrementChance << ","
	//	<< m_ItemConfig.DurabilityRepairCostFactor << ","
	//	<< m_ItemConfig.DurabilityConst << ","
	//	<< m_ItemConfig.HoldableDurabilityConst << ","
	//	<< m_ItemConfig.WearableDurabilityConst << ","
	//	<< m_ItemConfig.DeathDurabilityLossRatio << ","
	//	<< m_ItemConfig.ItemStatConst << ","
	//	<< m_ItemConfig.HoldableStatConst << ","
	//	<< m_ItemConfig.WearableStatConst << ","
	//	<< m_ItemConfig.StatValueConst << ","
	//	<< std::endl << std::endl;

	// ----------------->

	const char* command2 = "SELECT * FROM item_look_convert_required_items";
	stmt = sm.SqlPrepare(db, command2);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		ItemLookConvert rowData;
		
		rowData.Id = sqlite3_column_int(stmt, 1);
		rowData.RequiredItemId = sqlite3_column_int(stmt, 2);
		rowData.RequiredItemCount = sqlite3_column_int(stmt, 3);

		m_ItemLookConverts.insert({ rowData.Id, rowData });
	} sqlite3_finalize(stmt);

	// Debug, was correct
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (const auto& item : m_ItemLookConverts) {
	//	std::cout
	//
	//	<< item.second.Id << ", "
	//	<< item.second.RequiredItemId << ", "
	//	<< item.second.RequiredItemCount 
	//
	//	<< std::endl;
	//}
	
	const char* command3 = "SELECT * FROM item_look_convert_holdables";
	stmt = sm.SqlPrepare(db, command3);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		
		uint32 itemLookConvertId = sqlite3_column_int(stmt, 1);
		uint32 holdableId = sqlite3_column_int(stmt, 2);

		m_HoldableItemLookConverts.insert({ holdableId, itemLookConvertId });
	} sqlite3_finalize(stmt);

	// Debug - correct
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (const auto& item : m_HoldableItemLookConverts) {
	//	std::cout
	//
	//	<< item.first << ", "
	//	<< item.second << ", "
	//	
	//	<< std::endl;
	//}

	const char* command4 = "SELECT * FROM item_look_convert_wearables";
	stmt = sm.SqlPrepare(db, command4);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {

		uint32 itemLookConvertId = sqlite3_column_int(stmt, 1);
		uint32 wearableId = sqlite3_column_int(stmt, 2);

		m_WearableItemLookConverts.insert({ wearableId, itemLookConvertId });
	} sqlite3_finalize(stmt);

	// Debug - TODO: found one duplicate, check this
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (const auto& item : m_WearableItemLookConverts) {
	//	std::cout
	//
	//	<< item.first << ", "
	//	<< item.second << ", "
	//	
	//	<< std::endl;
	//}

	const char* command5 = "SELECT * FROM item_grades";
	stmt = sm.SqlPrepare(db, command5);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		GradeTemplate gradeTemplate;

		gradeTemplate.Grade = sqlite3_column_int(stmt, 2);

		gradeTemplate.HoldableDps = (float)sqlite3_column_double(stmt, 3);
		gradeTemplate.HoldableArmor = (float)sqlite3_column_double(stmt, 4);
		gradeTemplate.HoldableMagicDps = (float)sqlite3_column_double(stmt, 5);
		gradeTemplate.WearableArmor = (float)sqlite3_column_double(stmt, 6);
		gradeTemplate.WearableMagicResistance = (float)sqlite3_column_double(stmt, 7);
		gradeTemplate.Durability = (float)sqlite3_column_double(stmt, 10);

		gradeTemplate.UpgradeRatio = sqlite3_column_int(stmt, 13);
		gradeTemplate.StatMultiplier = sqlite3_column_int(stmt, 14);
		gradeTemplate.RefundMultiplier = sqlite3_column_int(stmt, 15);
		gradeTemplate.EnchantSuccessRatio = sqlite3_column_int(stmt, 16);
		gradeTemplate.EnchantGreatSuccessRatio = sqlite3_column_int(stmt, 17);
		gradeTemplate.EnchantBreakRatio = sqlite3_column_int(stmt, 18);
		gradeTemplate.EnchantDowngradeRatio = sqlite3_column_int(stmt, 19);
		gradeTemplate.EnchantCost = sqlite3_column_int(stmt, 20);

		gradeTemplate.HoldableHealDps = (float)sqlite3_column_double(stmt, 21);

		gradeTemplate.EnchantDowngradeMin = sqlite3_column_int(stmt, 22);
		gradeTemplate.EnchantDowngradeMax = sqlite3_column_int(stmt, 23);
		gradeTemplate.CurrencyId = sqlite3_column_int(stmt, 24);
		
		m_GradeTemplates.insert({ gradeTemplate.Grade, gradeTemplate });
	} sqlite3_finalize(stmt);

	// Debug - seems correct
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (const auto& item : m_GradeTemplates) {
	//	std::cout
	//
	//	<< item.second.Grade << ", "
	//
	//	<< item.second.HoldableDps << ", "
	//	<< item.second.HoldableArmor << ", "
	//	<< item.second.HoldableMagicDps << ", "
	//	<< item.second.WearableArmor << ", "
	//	<< item.second.WearableMagicResistance << ", "
	//	<< item.second.Durability << ", "
	//
	//	<< item.second.UpgradeRatio << ", "
	//	<< item.second.StatMultiplier << ", "
	//	<< item.second.RefundMultiplier << ", "
	//	<< item.second.EnchantSuccessRatio << ", "
	//	<< item.second.EnchantGreatSuccessRatio << ", "
	//	<< item.second.EnchantBreakRatio << ", "
	//	<< item.second.EnchantDowngradeRatio << ", "
	//	<< item.second.EnchantCost << ", "
	//
	//	<< item.second.HoldableHealDps << ", "
	//
	//	<< item.second.EnchantDowngradeMin << ", "
	//	<< item.second.EnchantDowngradeMax << ", "
	//	<< item.second.CurrencyId << ", "
	//	
	//	<< std::endl;
	//}

	const char* command6 = "SELECT * FROM holdables";
	stmt = sm.SqlPrepare(db, command6);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		Holdable holdable;

		holdable.Id = sqlite3_column_int(stmt, 0);
		holdable.KindId = sqlite3_column_int(stmt, 1);
		holdable.Speed = sqlite3_column_int(stmt, 2);
		holdable.ExtraDamagePierceFactor = sqlite3_column_int(stmt, 3);
		holdable.ExtraDamageSlashFactor = sqlite3_column_int(stmt, 4);
		holdable.ExtraDamageBluntFactor = sqlite3_column_int(stmt, 5);
		holdable.MaxRange = sqlite3_column_int(stmt, 6);
		holdable.Angle = sqlite3_column_int(stmt, 7);
		holdable.EnchantedDps1000 = sqlite3_column_int(stmt, 11);
		holdable.SlotTypeId = sqlite3_column_int(stmt, 12);
		holdable.DamageScale = sqlite3_column_int(stmt, 23);

		//holdable.FormulaDps = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 25));
		//holdable.FormulaMDps = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 26));
		//holdable.FormulaArmor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 27));

		holdable.MinRange = sqlite3_column_int(stmt, 28);
		holdable.SheathePriority = sqlite3_column_int(stmt, 30);
		holdable.DurabilityRatio = (float)sqlite3_column_double(stmt, 31);
		holdable.RenewCategory = sqlite3_column_int(stmt, 32);
		holdable.ItemProcId = sqlite3_column_int(stmt, 33);
		holdable.StatMultiplier = sqlite3_column_int(stmt, 34);
		
		//holdable.FormulaHDps = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 27));

		m_Holdables.insert({ holdable.Id, holdable });
	} sqlite3_finalize(stmt);

	// Debug - Looks ok
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (const auto& item : m_Holdables) {
	//	std::cout
	//
	//	<< item.second.Id << ", "
	//	<< item.second.KindId << ", "
	//	<< item.second.Speed << ", "
	//	<< item.second.ExtraDamagePierceFactor << ", "
	//	<< item.second.ExtraDamageSlashFactor << ", "
	//	<< item.second.ExtraDamageBluntFactor << ", "
	//	<< item.second.MaxRange << ", "
	//	<< item.second.Angle << ", "
	//	<< item.second.EnchantedDps1000 << ", "
	//	<< item.second.SlotTypeId << ", "
	//	<< item.second.DamageScale << ", "
	//	<< item.second.MinRange << ", "
	//	<< item.second.SheathePriority << ", "
	//	<< item.second.DurabilityRatio << ", "
	//	<< item.second.RenewCategory << ", "
	//	<< item.second.ItemProcId << ", "
	//	<< item.second.StatMultiplier << ", "
	//	
	//	<< std::endl;
	//}

	const char* command7 = "SELECT * FROM wearables";
	stmt = sm.SqlPrepare(db, command7);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		Wearable wearable;
		wearable.TypeId = sqlite3_column_int(stmt, 1);
		wearable.SlotTypeId = sqlite3_column_int(stmt, 2);
		wearable.ArmorBp = sqlite3_column_int(stmt, 3);
		wearable.MagicResistanceBp = sqlite3_column_int(stmt, 4);

		m_Wearables.insert({ wearable.TypeId * 128 + wearable.SlotTypeId, wearable });
	} sqlite3_finalize(stmt);

	//// Debug - seems ok
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (const auto& item : m_Wearables) {
	//	std::cout
	//
	//	<< item.second.TypeId << ", "
	//	<< item.second.SlotTypeId << ", "
	//	<< item.second.ArmorBp << ", "
	//	<< item.second.MagicResistanceBp << ", "
	//	
	//	<< std::endl;
	//}

	const char* command8 = "SELECT * FROM wearable_kinds";
	stmt = sm.SqlPrepare(db, command8);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		WearableKind wearableKind;

		wearableKind.TypeId = sqlite3_column_int(stmt, 1);
		wearableKind.ArmorRatio = sqlite3_column_int(stmt, 2);
		wearableKind.MagicResistanceRatio = sqlite3_column_int(stmt, 3);
		wearableKind.FullBufId = sqlite3_column_int(stmt, 4);
		wearableKind.HalfBufId = sqlite3_column_int(stmt, 5);

		wearableKind.ExtraDamagePierce = sqlite3_column_int(stmt, 7);
		wearableKind.ExtraDamageSlash = sqlite3_column_int(stmt, 8);
		wearableKind.ExtraDamageBlunt = sqlite3_column_int(stmt, 9);

		wearableKind.DurabilityRatio = (float)sqlite3_column_double(stmt, 10);

		m_WearableKinds.insert({ wearableKind.TypeId, wearableKind });
	} sqlite3_finalize(stmt);

	//// Debug - Ok
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (const auto& item : m_WearableKinds) {
	//	std::cout
	//
	//	<< item.second.TypeId << ", "
	//	<< item.second.ArmorRatio << ", "
	//	<< item.second.MagicResistanceRatio << ", "
	//	<< item.second.FullBufId << ", "
	//	<< item.second.HalfBufId << ", "
	//	<< item.second.ExtraDamagePierce << ", "
	//	<< item.second.ExtraDamageSlash << ", "
	//	<< item.second.ExtraDamageBlunt << ", "
	//	<< item.second.DurabilityRatio
	//	
	//	<< std::endl;
	//}

	const char* command9 = "SELECT * FROM wearable_slots";
	stmt = sm.SqlPrepare(db, command9);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		WearableSlot wearableSlot;
		wearableSlot.SlotTypeId = sqlite3_column_int(stmt, 1);
		wearableSlot.Coverage = sqlite3_column_int(stmt, 2);
		
		m_WearableSlots.insert({ wearableSlot.SlotTypeId, wearableSlot });
	} sqlite3_finalize(stmt);

	//// Debug - Ok
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (const auto& item : m_WearableSlots) {
	//	std::cout
	//	<< item.second.SlotTypeId << ", "
	//	<< item.second.Coverage
	//	<< std::endl;
	//}

	const char* command10 = "SELECT * FROM equip_item_attr_modifiers";
	stmt = sm.SqlPrepare(db, command10);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) 
	{
		AttributeModifier aM;

		aM.Id = sqlite3_column_int(stmt, 0);
		aM.StrWeight = sqlite3_column_int(stmt, 2);
		aM.DexWeight = sqlite3_column_int(stmt, 3);
		aM.StaWeight = sqlite3_column_int(stmt, 4);
		aM.IntWeight = sqlite3_column_int(stmt, 5);
		aM.SpiWeight = sqlite3_column_int(stmt, 6);

		m_AttributeModifiers.insert({ aM.Id, aM });
	} sqlite3_finalize(stmt);

	//// Debug - Ok
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (const auto& item : m_AttributeModifiers) {
	//	std::cout
	//	<< item.second.Id << ", "
	//	<< item.second.StrWeight << ", "
	//	<< item.second.DexWeight << ", "
	//	<< item.second.StaWeight << ", "
	//	<< item.second.IntWeight << ", "
	//	<< item.second.SpiWeight
	//	<< std::endl;
	//}

	const char* command11 = "SELECT * FROM item_procs";
	stmt = sm.SqlPrepare(db, command11);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		ItemProcTemplate iPT;

		iPT.Id = sqlite3_column_int(stmt, 0);
		iPT.SkillId = sqlite3_column_int(stmt, 3);
		iPT.ChanceKindId = (ProcChanceKind)sqlite3_column_int(stmt, 4);
		iPT.ChanceRate = sqlite3_column_int(stmt, 5);
		iPT.ChanceParam = sqlite3_column_int(stmt, 6);
		iPT.CooldownSec = sqlite3_column_int(stmt, 7);

		const unsigned char* isFinisher = sqlite3_column_text(stmt, 8);
		if (isFinisher && isFinisher[0]) {
			iPT.Finisher = isFinisher[0] == 't' ? true : false;
		}

		iPT.ItemLevelBasedChanceBonus = sqlite3_column_int(stmt, 9);
		
		m_ItemProcTemplates.insert({ iPT.Id, iPT });
	} sqlite3_finalize(stmt);

	//// Debug - Ok
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (const auto& item : m_ItemProcTemplates) {
	//	std::cout
	//	<< item.second.Id << ", "
	//	<< item.second.SkillId << ", "
	//	<< (int32)item.second.ChanceKindId << ", "
	//	<< item.second.ChanceRate << ", "
	//	<< item.second.ChanceParam << ", "
	//	<< item.second.CooldownSec << ", "
	//	<< item.second.Finisher << ", "
	//	<< item.second.ItemLevelBasedChanceBonus
	//	<< std::endl;
	//}

	const char* command12 = "SELECT * FROM equip_item_set_bonuses";
	stmt = sm.SqlPrepare(db, command12);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		EquipItemSet eIS;
		eIS.Id = sqlite3_column_int(stmt, 1);
		if (m_EquipItemSets.contains(eIS.Id) == false) {
			m_EquipItemSets.insert({ eIS.Id, eIS });
		}

		EquipItemSetBonus bonus;
		bonus.NumPieces = sqlite3_column_int(stmt, 2);
		bonus.BuffId = sqlite3_column_int(stmt, 3);
		bonus.ItemProcId = sqlite3_column_int(stmt, 4);
		
		if (bonus.BuffId != 0 || bonus.ItemProcId != 0) {
			m_EquipItemSets[eIS.Id].Bonuses.push_back(bonus);
		}
		
	} sqlite3_finalize(stmt);

	// Debug - Ok
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (const auto& item : m_EquipItemSets) {
	//	std::cout << item.first << " : ";
	//
	//	for (const auto& bonus : item.second.Bonuses) {
	//		std::cout
	//		<< bonus.NumPieces << " , "
	//		<< bonus.BuffId << " , "
	//		<< bonus.ItemProcId << " , ";
	//	}
	//
	//	std::cout << std::endl;
	//}

	const char* command13 = "SELECT * FROM item_armors";
	stmt = sm.SqlPrepare(db, command13);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		ArmorTemplate* t = new ArmorTemplate();
		t->Id = sqlite3_column_int(stmt, 1);

		uint32 slotTypeId = sqlite3_column_int(stmt, 2);
		uint32 typeId = sqlite3_column_int(stmt, 3);
		t->WearableTemplate = &m_Wearables[typeId * 128 + slotTypeId];
		t->KindTemplate = &m_WearableKinds[typeId];
		t->SlotTemplate = &m_WearableSlots[slotTypeId];

		SQLiteUtility::SetBool(t->BaseEnchantable, sqlite3_column_text(stmt, 4));
		t->ModSetId = sqlite3_column_int(stmt, 6);
		t->EquipItemSetId = sqlite3_column_int(stmt, 7);
		SQLiteUtility::SetBool(t->Repairable, sqlite3_column_text(stmt, 9));
		t->DurabilityMultiplier = sqlite3_column_int(stmt, 10);
		SQLiteUtility::SetBool(t->BaseEquipment, sqlite3_column_text(stmt, 11));

		t->RechargeBuffId = sqlite3_column_int(stmt, 12);
		t->ChargeLifetime = sqlite3_column_int(stmt, 13);
		t->ChargeCount = sqlite3_column_int(stmt, 14);

		t->ItemLookConvert = m_ItemLookConverts.contains(slotTypeId) 
			? &m_ItemLookConverts[slotTypeId] : nullptr;

		m_ItemTemplates.insert({ t->Id, t });
		
	} sqlite3_finalize(stmt);

	int32 numArmorTemplates = (int32)m_ItemTemplates.size();
	spdlog::info("[ItemManager] - Loaded ArmorTemplates: {}", numArmorTemplates);

	//// Debug - Ok
	//std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	//for (auto& _item : m_ItemTemplates) {
	//	if (&_item.second == nullptr) continue;
	//	
	//	if (_item.first < 1176 || _item.first > 3007) { continue; }
	//	std::cout << "Entry: " << _item.first << std::endl;
	//	//ItemTemplate& itemTemplate = _item.second;
	//	ArmorTemplate* item = dynamic_cast<ArmorTemplate*>(_item.second);
	//	if (item == nullptr) { continue; }
	//	std::cout //<< _item.first << " : "
	//	
	//	<< item->Id << ", "
	//	<< item->BaseEnchantable << ", "
	//	<< item->ModSetId << ", "
	//	<< item->EquipItemSetId << ", "
	//	<< item->Repairable << ", "
	//	<< item->DurabilityMultiplier << ", "
	//	<< item->BaseEquipment << ", "
	//	<< item->RechargeBuffId << ", "
	//	<< item->ChargeLifetime << ", "
	//	<< item->ChargeCount << ", "
	//	
	//	<< std::endl;
	//}

	const char* command14 = "SELECT * FROM item_weapons";
	stmt = sm.SqlPrepare(db, command14);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		WeaponTemplate& t = *(new WeaponTemplate);
		t.Id = sqlite3_column_int(stmt, 1);

		int32 holdableId = sqlite3_column_int(stmt, 4);

		SQLiteUtility::SetBool(t.BaseEnchantable, sqlite3_column_text(stmt, 3));
		t.HoldableTemplate = &m_Holdables[holdableId];
		t.ModSetId = sqlite3_column_int(stmt, 5);
		t.EquipItemSetId = sqlite3_column_int(stmt, 6);
		SQLiteUtility::SetBool(t.Repairable, sqlite3_column_text(stmt, 8));
		t.DurabilityMultiplier = sqlite3_column_int(stmt, 9);
		SQLiteUtility::SetBool(t.BaseEquipment, sqlite3_column_text(stmt, 10));
		t.RechargeBuffId = sqlite3_column_int(stmt, 13);
		t.ChargeLifetime = sqlite3_column_int(stmt, 14);
		t.ChargeCount = sqlite3_column_int(stmt, 15);
		t.ItemLookConvert = m_HoldableItemLookConverts.contains(holdableId)
			? &m_ItemLookConverts[m_HoldableItemLookConverts[holdableId]] : nullptr;

		m_ItemTemplates.insert({ t.Id, &t });

	} sqlite3_finalize(stmt);

	int32 numWeaponTemplates = (int32)m_ItemTemplates.size() - numArmorTemplates;
	
	spdlog::info("[ItemManager] - Loaded WeaponTemplates: {}", numWeaponTemplates);

	/* Debug - Ok
	std::cout << std::endl << "[ItemManager][DEBUG] - " << std::endl;
	for (auto& _item : m_ItemTemplates) {
		if (&_item.second == nullptr) continue;
		
		if (_item.first < 5 || _item.first > 511) { continue; }
		//std::cout << "Entry: " << _item.first << std::endl;
		//ItemTemplate& itemTemplate = _item.second;
		WeaponTemplate* item = dynamic_cast<WeaponTemplate*>(_item.second);
		if (item == nullptr) { continue; }
		std::cout //<< _item.first << " : "
		
		<< item->Id << ", "
		<< item->BaseEnchantable << ", "
		<< item->ModSetId << ", "
		<< item->EquipItemSetId << ", "
		<< item->Repairable << ", "
		<< item->DurabilityMultiplier << ", "
		<< item->BaseEquipment << ", "
		<< item->RechargeBuffId << ", "
		<< item->ChargeLifetime << ", "
		<< item->ChargeCount << ", "
		
		<< std::endl;
	}
	*/

	const char* command15 = "SELECT * FROM item_accessories";
	stmt = sm.SqlPrepare(db, command15);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		AccessoryTemplate& t = *(new AccessoryTemplate);
		t.Id = sqlite3_column_int(stmt, 1);

		uint32 slotTypeId = sqlite3_column_int(stmt, 2);
		uint32 typeId = sqlite3_column_int(stmt, 3);

		t.WearableTemplate = &m_Wearables[typeId * 128 + slotTypeId];
		t.KindTemplate = &m_WearableKinds[typeId];
		t.SlotTemplate = &m_WearableSlots[slotTypeId];

		t.ModSetId = sqlite3_column_int(stmt, 4);
		t.EquipItemSetId = sqlite3_column_int(stmt, 5);
		SQLiteUtility::SetBool(t.Repairable, sqlite3_column_text(stmt, 7));
		t.DurabilityMultiplier = sqlite3_column_int(stmt, 8);
		t.RechargeBuffId = sqlite3_column_int(stmt, 9);
		t.ChargeLifetime = sqlite3_column_int(stmt, 10);
		t.ChargeCount = sqlite3_column_int(stmt, 11);

		m_ItemTemplates.insert({ t.Id, &t });

	} sqlite3_finalize(stmt);

	int32 numAccessoryTemplates = (int32)m_ItemTemplates.size() - numArmorTemplates - numWeaponTemplates;
	spdlog::info("[ItemManager] - Loaded AccessoryTemplate: {}", numAccessoryTemplates);

	// ------>
	
	const char* command16 = "SELECT * FROM item_summon_mates";
	stmt = sm.SqlPrepare(db, command16);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		SummonMateTemplate& t = *(new SummonMateTemplate);
		t.Id = sqlite3_column_int(stmt, 1);	// item_id
		t.NpcId = sqlite3_column_int(stmt, 2); // npc_id

		m_ItemTemplates.insert({ t.Id, &t });

	} sqlite3_finalize(stmt);

	int32 numSummonMateTemplate = (int32)m_ItemTemplates.size() - numArmorTemplates
		- numWeaponTemplates - numAccessoryTemplates;
	
	spdlog::info("[ItemManager] - Loaded SummonMateTemplate: {}", numSummonMateTemplate);
	// ------>
	
	const char* command17 = "SELECT * FROM item_summon_slaves";
	stmt = sm.SqlPrepare(db, command17);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		SummonSlaveTemplate& t = *(new SummonSlaveTemplate);
		t.Id = sqlite3_column_int(stmt, 1);	// item_id
		t.SlaveId = sqlite3_column_int(stmt, 2); // slave_id

		m_ItemTemplates.insert({ t.Id, &t });

	} sqlite3_finalize(stmt);

	int32 numSummonSlaveTemplate = (int32)m_ItemTemplates.size() - numArmorTemplates
		- numWeaponTemplates - numAccessoryTemplates - numSummonMateTemplate;

	spdlog::info("[ItemManager] - Loaded SummonSlaveTemplate: {}", numSummonSlaveTemplate);
	// ------>

	const char* command18 = "SELECT * FROM item_body_parts";
	stmt = sm.SqlPrepare(db, command18);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		uint32 _id = sqlite3_column_int(stmt, 1);
		if (_id == 0) { continue; }

		BodyPartTemplate& t = *(new BodyPartTemplate);
		t.Id = _id;	// item_id
		t.ItemId = _id;
		t.SlotTypeId = sqlite3_column_int(stmt, 2);
		t.ModelId = sqlite3_column_int(stmt, 3);
		SQLiteUtility::SetBool(t.NpcOnly, sqlite3_column_text(stmt, 11));
		SQLiteUtility::SetBool(t.BeautyShopOnly, sqlite3_column_text(stmt, 12));
		
		m_ItemTemplates.insert({ t.Id, &t });

	} sqlite3_finalize(stmt);

	int32 numBodyPartTemplate = (int32)m_ItemTemplates.size() - numArmorTemplates - numWeaponTemplates
		- numAccessoryTemplates - numSummonMateTemplate - numSummonSlaveTemplate;

	spdlog::info("[ItemManager] - Loaded BodyPartTemplate: {}", numBodyPartTemplate);
	// ------>

	const char* command19 = "SELECT * FROM item_enchanting_gems";
	stmt = sm.SqlPrepare(db, command19);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		uint32 _id = sqlite3_column_int(stmt, 1);
		
		RuneTemplate& t = *(new RuneTemplate);
		t.Id = _id;	// item_id
		t.EquipSlotGroupId = sqlite3_column_int(stmt, 2);
		t.EquipLevel = sqlite3_column_int(stmt, 4);
		t.ItemGradeId = sqlite3_column_int(stmt, 5);
		
		m_ItemTemplates.insert({ t.Id, &t });

	} sqlite3_finalize(stmt);

	int32 numRuneTemplate = (int32)m_ItemTemplates.size() - numArmorTemplates - numWeaponTemplates
		- numAccessoryTemplates - numSummonMateTemplate - numSummonSlaveTemplate - numBodyPartTemplate;

	spdlog::info("[ItemManager] - Loaded RuneTemplate: {}", numRuneTemplate);
	// ------>

	const char* command20 = "SELECT * FROM item_backpacks";
	stmt = sm.SqlPrepare(db, command20);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		uint32 _id = sqlite3_column_int(stmt, 1);

		BackpackTemplate& t = *(new BackpackTemplate);
		t.Id = _id;	// item_id
		t.AssetId = sqlite3_column_int(stmt, 2);	// asset_id
		t.BackpackType = (BackpackType)sqlite3_column_int(stmt, 3);
		t.DeclareSiegeZoneGroupId = sqlite3_column_int(stmt, 4);
		SQLiteUtility::SetBool(t.Heavy, sqlite3_column_text(stmt, 5));
		t.Asset2Id = sqlite3_column_int(stmt, 6);
		SQLiteUtility::SetBool(t.NormalSpeciality, sqlite3_column_text(stmt, 7));
		SQLiteUtility::SetBool(t.UseAsStat, sqlite3_column_text(stmt, 8));
		t.SkinKindId = sqlite3_column_int(stmt, 9);

		m_ItemTemplates.insert({ t.Id, &t });

	} sqlite3_finalize(stmt);

	int32 numBackpackTemplate = (int32)m_ItemTemplates.size() - numArmorTemplates
		- numWeaponTemplates - numAccessoryTemplates - numSummonMateTemplate 
		- numSummonSlaveTemplate - numBodyPartTemplate - numRuneTemplate;

	spdlog::info("[ItemManager] - Loaded BackpackTemplate: {}", numBackpackTemplate);
	// TEMP fix for crest Ink/Stamp/Music
	UccTemplate& crestInk = *(new UccTemplate);
	crestInk.Id = CREST_INK;
	m_ItemTemplates.insert({ crestInk.Id, &crestInk });

	UccTemplate& crestStamp = *(new UccTemplate);
	crestStamp.Id = CREST_STAMP;
	m_ItemTemplates.insert({ crestStamp.Id, &crestStamp });

	MusicSheetTemlate& musicSheet = *(new MusicSheetTemlate);
	musicSheet.Id = SHEET_MUSIC;
	m_ItemTemplates.insert({ musicSheet.Id, &musicSheet });
	// --->

	int32 numBeforeItems = (int32)m_ItemTemplates.size();

	const char* command21 = "SELECT * FROM items";
	stmt = sm.SqlPrepare(db, command21);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		uint32 _id = sqlite3_column_int(stmt, 0);	// id

		bool doInsert = false;

		ItemTemplate* _t;

		if (m_ItemTemplates.contains(_id) == false) {
			doInsert = true;
			_t = new ItemTemplate;
		}
		else {
			_t = m_ItemTemplates[_id];
		}

		ItemTemplate& t = *_t;

		//ItemTemplate& t = m_ItemTemplates.contains(_id)
		//	? *m_ItemTemplates[_id] : *new ItemTemplate;

		t.Id = _id;	// id , which is item_id for the others
		SQLiteUtility::SetString(t.Name, sqlite3_column_text(stmt, 1));
		t.CategoryId = sqlite3_column_int(stmt, 2); // category_id
		t.Level = sqlite3_column_int(stmt, 3); // level
		t.Price = sqlite3_column_int(stmt, 5); // price
		t.Refund = sqlite3_column_int(stmt, 6); // refund
		t.BindType = (ItemBindType)sqlite3_column_int(stmt, 7); // bind_id
		t.PickupLimit = sqlite3_column_int(stmt, 8); // pickup_limit
		t.MaxCount = sqlite3_column_int(stmt, 9); // max_stack_size
		SQLiteUtility::SetBool(t.Sellable, sqlite3_column_text(stmt, 11)); // sellable
		t.UseSkillId = sqlite3_column_int(stmt, 12); // use_skill_id
		SQLiteUtility::SetBool(t.UseSkillAsReagent, sqlite3_column_text(stmt, 13)); // use_skill_as_reagent
		t.ImplId = (ItemTypeId)sqlite3_column_int(stmt, 14); // impl_id
		t.BuffId = sqlite3_column_int(stmt, 17); // buff_id
		SQLiteUtility::SetBool(t.Gradeable, sqlite3_column_text(stmt, 18)); // gradeable
		SQLiteUtility::SetBool(t.LootMulti, sqlite3_column_text(stmt, 19)); // loot_multi
		t.LootQuestId = sqlite3_column_int(stmt, 20); // loot_quest_id
		t.HonorPrice = sqlite3_column_int(stmt, 23); // honor_price
		t.ExpAbsLifetime = sqlite3_column_int(stmt, 24); // exp_abs_lifetime
		t.ExpOnlineLifetime = sqlite3_column_int(stmt, 25); // exp_online_lifetime
		//time_t expDate = 
		//t.ExpDate = 
		//const unsigned char* timeString = sqlite3_column_text(stmt, 26); 
		SQLiteUtility::SetTime(t.ExpDate, sqlite3_column_text(stmt, 26)); // exp_date

		t.LevelReq = sqlite3_column_int(stmt, 28); // level_requirement
		t.AuctionCategoryA = sqlite3_column_int(stmt, 30); // auction_a_category_id
		t.AuctionCategoryB = sqlite3_column_int(stmt, 31); // auction_b_category_id
		t.AuctionCategoryC = sqlite3_column_int(stmt, 32); // auction_c_category_id
		t.LevelLimit = sqlite3_column_int(stmt, 33); // level_limit
		t.FixedGrade = sqlite3_column_int(stmt, 34); // fixed_grade
		SQLiteUtility::SetBool(t.Disenchantable, sqlite3_column_text(stmt, 35)); // disenchantable
		t.LivingPointPrice = sqlite3_column_int(stmt, 36); // living_point_price
		t.CharGender = (uint8)sqlite3_column_int(stmt, 40); // char_gender_id

		if (doInsert) {
			m_ItemTemplates.insert({ t.Id, &t });
		}
		
	} sqlite3_finalize(stmt);

	int32 numAfter = (int32)m_ItemTemplates.size() - numBeforeItems;

	spdlog::info("[ItemManager] - Loaded Additional Items: {}", numAfter);
	spdlog::info("[ItemManager] - Total Loaded Items: {}", m_ItemTemplates.size());

	const char* command22 = "SELECT * FROM equip_slot_enchanting_costs";
	stmt = sm.SqlPrepare(db, command22);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		EquipSlotEnchantingCost eC;

		eC.Id = sqlite3_column_int(stmt, 0);
		eC.SlotTypeId = sqlite3_column_int(stmt, 1);
		eC.Cost = sqlite3_column_int(stmt, 2);

		m_EnchantingCosts.insert({ eC.Id, eC });

	} sqlite3_finalize(stmt);

	// --->
	

	const char* command23 = "SELECT * FROM item_grade_enchanting_supports";
	stmt = sm.SqlPrepare(db, command23);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		ItemGradeEnchantingSupport x;

		x.Id = sqlite3_column_int(stmt, 0);
		x.ItemId = sqlite3_column_int(stmt, 1);
		x.RequireGradeMax = sqlite3_column_int(stmt, 2);
		x.AddSuccessRatio = sqlite3_column_int(stmt, 3);
		x.AddSuccessMul = sqlite3_column_int(stmt, 4);
		x.AddGreatSuccessRatio = sqlite3_column_int(stmt, 5);
		x.AddGreatSuccessMul = sqlite3_column_int(stmt, 6);
		x.AddBreakRatio = sqlite3_column_int(stmt, 7);
		x.AddBreakMul = sqlite3_column_int(stmt, 8);
		x.AddDowngradeRatio = sqlite3_column_int(stmt, 9);
		x.AddDowngradeMul = sqlite3_column_int(stmt, 10);
		x.AddGreatSuccessGrade = sqlite3_column_int(stmt, 11);
		x.RequireGradeMin = sqlite3_column_int(stmt, 12);
		
		m_EnchantingSupports.insert({ x.Id, x });

	} sqlite3_finalize(stmt);

	// --->

	const char* command24 = "SELECT * FROM item_socket_chances";
	stmt = sm.SqlPrepare(db, command24);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		
		uint32 numSockets = sqlite3_column_int(stmt, 1);
		uint32 chance = sqlite3_column_int(stmt, 2);
		
		m_SocketChance.insert({ numSockets, chance });

	} sqlite3_finalize(stmt);

	// --->

	const char* command25 = "SELECT * FROM item_cap_scales";
	stmt = sm.SqlPrepare(db, command25);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		ItemCapScale x;

		x.Id = sqlite3_column_int(stmt, 0);
		x.SkillId = sqlite3_column_int(stmt, 2);
		x.ScaleMin = sqlite3_column_int(stmt, 3);
		x.ScaleMax = sqlite3_column_int(stmt, 4);

		m_ItemCapScales.insert({ x.Id, x });

	} sqlite3_finalize(stmt);

	spdlog::warn("[ItemManager] - BE AWARE: There are more to be loaded in the future...");

	sqlite3_close(db);
	return true;
}
