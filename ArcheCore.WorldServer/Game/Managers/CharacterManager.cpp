#pragma once

#include "CharacterManager.h"
#include "common_utils.h"
#include "Managers/FactionManager.h"
#include "Managers/ItemManager.h"
#include "Managers/SqLiteManager.h"
#include "SkillManager.h"
#include "../WorldSession.h"
#include "Utility/JsonUtility.h"
#include "WorldManager.h"
#include "Entity/Object.h"
#include "Entity/Transform.h"

#include <sqlite3.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace e;

void CharacterManager::Load() {
	//m_Templates = new std::map<uint8_t, uint16_t>();
	std::unordered_map<uint32_t, uint8_t> temp;

	SqLiteManager& sm = SqLiteManager::Instance();
	sqlite3* db = sm.SqlOpen();

	/* Create SQL statement */
	const char* command = "SELECT * FROM characters";
	sqlite3_stmt* stmt = sm.SqlPrepare(db, command);

	//spdlog::info("[CharacterManager] - Loading Character Templates...");

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		// Reading each row found under "characters"
		int id = sqlite3_column_int(stmt, 0);
		int race = sqlite3_column_int(stmt, 1);
		int gender = sqlite3_column_int(stmt, 2);
		int modelId = sqlite3_column_int(stmt, 3);
		int factionId = sqlite3_column_int(stmt, 4);
		int zoneId = sqlite3_column_int(stmt, 5);
		int returnDistrictId = sqlite3_column_int(stmt, 8);
		int resurecctDistrictId = sqlite3_column_int(stmt, 9);

		CharacterTemplate charTemplate;
		charTemplate.Race = (Race)race;
		charTemplate.Gender = Gender(gender);
		charTemplate.ModelId = modelId;
		charTemplate.FactionId = factionId;
		charTemplate.ZoneId = zoneId;
		charTemplate.ReturnDistrictId = returnDistrictId;
		charTemplate.ResurrectionDistrictId = resurecctDistrictId;

		std::string command2 = "SELECT * FROM item_body_parts WHERE model_id = " + std::to_string(modelId);
		//std::cout << commandString << std::endl;

		sqlite3_stmt* stmt2 = sm.SqlPrepare(db, command2.c_str());

		while (int ret_code2 = sqlite3_step(stmt2) == SQLITE_ROW)
		{
			int itemId = sqlite3_column_int(stmt2, 1);
			int slotId = sqlite3_column_int(stmt2, 2) - 23;
			//std::cout << "SQLite: Read Row, " << "ItemId: " << itemId << " SlotId: " << slotId << std::endl;
			charTemplate.Items[slotId] = itemId;

		} sqlite3_finalize(stmt2);

		uint8_t templateId = (uint8_t)16 * (uint8_t)charTemplate.Gender + (uint8_t)charTemplate.Race;
		m_Templates.insert({ templateId, charTemplate });
		temp.insert({ id, templateId });
	} sqlite3_finalize(stmt);

	spdlog::info("[CharacterManager] - Loaded Character Templates: {}", m_Templates.size());

	/*
	// Debug, seems correct
	std::cout << std::endl << "[SQLite][DEBUG] - " << std::endl;
	for (const auto& item : m_Templates) {
		std::cout
	
		<< (int32)item.second.Race << ", "
		<< (int32)item.second.Gender << ", "
		<< (int32)item.second.ModelId << ", "
		<< (int32)item.second.FactionId << ", "
		<< (int32)item.second.ZoneId << ", "
		<< (int32)item.second.ReturnDistrictId << ", "
		<< item.second.ResurrectionDistrictId
		<< std::endl;

		for (size_t i = 0; i < 7; i++)
		{
			std::cout << "slot: " << i
				<< " id: " << item.second.Items[i] << std::endl;
		}

	}
	*/
	// ----------------->
	// Character Buffs
	const char* command3 = "SELECT * FROM character_buffs";
	stmt = sm.SqlPrepare(db, command3);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		int characterId = sqlite3_column_int(stmt, 1);
		int buffId = sqlite3_column_int(stmt, 2);
		m_Templates[temp[characterId]].Buffs.push_back(buffId);
	} sqlite3_finalize(stmt);

	// ----------------->
	// 
	// Character Supplies
	const char* command4 = "SELECT * FROM character_supplies";
	stmt = sm.SqlPrepare(db, command4);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		int abilityId = sqlite3_column_int(stmt, 1);
		int itemId = sqlite3_column_int(stmt, 2);
		int amount = sqlite3_column_int(stmt, 3);
		int gradeId = sqlite3_column_int(stmt, 4);

		AbilitySupplyItem item;
		item.Id = itemId;
		item.Amount = amount;
		item.Grade = gradeId;

		AbilityItems abilityItems;
		abilityItems.Supplies.push_back(item);

		m_AbilityItems.insert({ abilityId, abilityItems });
	} sqlite3_finalize(stmt);
	// ----------------->
	// 
	// Character Equip Packs
	const char* command5 = "SELECT * FROM character_equip_packs";
	stmt = sm.SqlPrepare(db, command5);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		int abilityId = sqlite3_column_int(stmt, 1);
		int newbieClothPackId = sqlite3_column_int(stmt, 2);
		int newbieWeaponPackId = sqlite3_column_int(stmt, 3);

		EquipItemsTemplate eqItemTemp = {};
		AbilityItems aItems = { (uint8_t)abilityId, eqItemTemp };

		if (newbieClothPackId > 0) {
			std::string command6 = "SELECT * FROM equip_pack_cloths WHERE id = " + std::to_string(newbieClothPackId);
			sqlite3_stmt* stmt2 = sm.SqlPrepare(db, command6.c_str());

			while (int ret_code2 = sqlite3_step(stmt2) == SQLITE_ROW)
			{
				aItems.Items.Headgear = sqlite3_column_int(stmt2, 2);
				aItems.Items.Necklace = sqlite3_column_int(stmt2, 3);
				aItems.Items.Shirt = sqlite3_column_int(stmt2, 4);
				aItems.Items.Belt = sqlite3_column_int(stmt2, 5);
				aItems.Items.Pants = sqlite3_column_int(stmt2, 6);
				aItems.Items.Gloves = sqlite3_column_int(stmt2, 7);
				aItems.Items.Shoes = sqlite3_column_int(stmt2, 8);
				aItems.Items.Bracelet = sqlite3_column_int(stmt2, 9);
				aItems.Items.HeadgearGrade = sqlite3_column_int(stmt2, 10);
				aItems.Items.NecklaceGrade = sqlite3_column_int(stmt2, 11);
				aItems.Items.ShirtGrade = sqlite3_column_int(stmt2, 12);
				aItems.Items.BeltGrade = sqlite3_column_int(stmt2, 13);
				aItems.Items.PantsGrade = sqlite3_column_int(stmt2, 14);
				aItems.Items.GlovesGrade = sqlite3_column_int(stmt2, 15);
				aItems.Items.ShoesGrade = sqlite3_column_int(stmt2, 16);
				aItems.Items.BraceletGrade = sqlite3_column_int(stmt2, 17);
				aItems.Items.Back = sqlite3_column_int(stmt2, 18);
				aItems.Items.BackGrade = sqlite3_column_int(stmt2, 19);
				aItems.Items.Cosplay = sqlite3_column_int(stmt2, 20);
				aItems.Items.CosplayGrade = sqlite3_column_int(stmt2, 21);
				aItems.Items.Undershirt = sqlite3_column_int(stmt2, 22);
				aItems.Items.UndershirtGrade = sqlite3_column_int(stmt2, 23);
				aItems.Items.Underpants = sqlite3_column_int(stmt2, 24);
				aItems.Items.UnderpantsGrade = sqlite3_column_int(stmt2, 25);
			} sqlite3_finalize(stmt2);
		}

		if (newbieWeaponPackId > 0) {
			std::string command7 = "SELECT * FROM equip_pack_weapons WHERE id = " + std::to_string(newbieWeaponPackId);
			sqlite3_stmt* stmt2 = sm.SqlPrepare(db, command7.c_str());
			while (int ret_code2 = sqlite3_step(stmt2) == SQLITE_ROW)
			{
				aItems.Items.Mainhand = sqlite3_column_int(stmt2, 2);
				aItems.Items.Offhand = sqlite3_column_int(stmt2, 3);
				aItems.Items.Ranged = sqlite3_column_int(stmt2, 4);
				aItems.Items.Instrument = sqlite3_column_int(stmt2, 5);
				aItems.Items.MainhandGrade = sqlite3_column_int(stmt2, 6);
				aItems.Items.OffhandGrade = sqlite3_column_int(stmt2, 7);
				aItems.Items.RangedGrade = sqlite3_column_int(stmt2, 8);
				aItems.Items.InstrumentGrade = sqlite3_column_int(stmt2, 9);
			} sqlite3_finalize(stmt2);
		}
		m_AbilityItems.insert({ aItems.Ability, aItems });
	} sqlite3_finalize(stmt);
	
	spdlog::info("[CharacterManager] - Loaded AbilityItems: {}", m_AbilityItems.size());
	// ----------------->
	// Bag Expands
	stmt = sm.SqlPrepare(db, "SELECT * FROM bag_expands");

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		Expand expand;

		// LMAO, what a fucking mess to read a simple bool...
		const char* isBank = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		if (isBank == NULL) {
			std::cout << "isBank: NULL!" << std::endl;
			break;
		}
		std::string isBankString(isBank);

		expand.IsBank = isBankString == "t" ? true : false;
		expand.Step = sqlite3_column_int(stmt, 2);
		expand.Price = sqlite3_column_int(stmt, 3);
		expand.ItemId = sqlite3_column_int(stmt, 4);
		expand.ItemCount = sqlite3_column_int(stmt, 5);
		expand.CurrencyId = sqlite3_column_int(stmt, 6);

		m_Expands[expand.Step].push_back(expand);
	} sqlite3_finalize(stmt);

	// ----------------->
	// Appellations
	stmt = sm.SqlPrepare(db, "SELECT id, buff_id FROM appellations");

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		AppellationTemplate templ = {};
		templ.Id = sqlite3_column_int(stmt, 0);
		templ.BuffId = sqlite3_column_int(stmt, 1);

		m_Appellations.insert({ templ.Id, templ });
	} sqlite3_finalize(stmt);

	spdlog::info("[CharacterManager] - Loaded Appellations: {}", m_Appellations.size());
	// ----------------->
	// Actability Groups
	stmt = sm.SqlPrepare(db, "SELECT * FROM actability_groups");

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		ActabilityTemplate t = {};

		t.Id = sqlite3_column_int(stmt, 0);

		const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		if (name == NULL) { std::cout << "_name: NULL!" << std::endl; break; }
		t.Name = std::string(name);
		t.UnitAttributeId = sqlite3_column_int(stmt, 5);

		m_Actabilites.insert({ t.Id, t });
	} sqlite3_finalize(stmt);

	spdlog::info("[CharacterManager] - Loaded Actability Groups: {}", m_Actabilites.size());
	// ----------------->
	// Expert Limits
	stmt = sm.SqlPrepare(db, "SELECT * FROM expert_limits ORDER BY up_limit ASC");
	int step = 0;
	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		ExpertLimit t;
		t.Id = sqlite3_column_int(stmt, 0);
		t.UpLimit = sqlite3_column_int(stmt, 2);
		t.ExpertLimitCount = (uint8_t)sqlite3_column_int(stmt, 3);
		t.Advantage = sqlite3_column_int(stmt, 4);
		t.CastAdvantage = sqlite3_column_int(stmt, 6);
		t.UpCurrencyId = sqlite3_column_int(stmt, 7);
		t.UpPrice = sqlite3_column_int(stmt, 8);
		t.DownCurrecyId = sqlite3_column_int(stmt, 9);
		t.DownPrice = sqlite3_column_int(stmt, 10);

		m_ExpertLimits.insert({ step++, t });
	} sqlite3_finalize(stmt);

	spdlog::info("[CharacterManager] - Loaded Expert Limits: {}", m_ExpertLimits.size());
	// ----------------->
	// Expand Expert Limits
	stmt = sm.SqlPrepare(db, "SELECT * FROM expand_expert_limits ORDER BY expand_count ASC");
	step = 0;
	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		ExpandExpertLimit t;
		t.Id = sqlite3_column_int(stmt, 0);
		t.ExpandCount = (uint8_t)sqlite3_column_int(stmt, 1);
		t.LifePoint = sqlite3_column_int(stmt, 2);
		t.ItemId = sqlite3_column_int(stmt, 3);
		t.ItemCount = sqlite3_column_int(stmt, 4);

		m_ExpandExpertLimits.insert({ step++, t });
	} sqlite3_finalize(stmt);

	spdlog::info("[CharacterManager] - Loaded Expand Expert Limits: {}", m_ExpandExpertLimits.size());
	// ----------------->
	sqlite3_close(db);

	std::string exePath = sm.GetFolderPath();
	std::string jsonString = std::string("data/CharTemplates.json");

	std::cout << std::endl << "[CharacterManager] - Json Path: " << (exePath + jsonString) << std::endl;

	// JSON stuff
	std::ifstream f((exePath + jsonString));
	json data = json::parse(f);
	if (data.empty()) {
		std::cout << "JSON ERROR: data is empty!" << std::endl;
		return;
	}

	std::vector<CharacterTemplateConfig> cTC;

	for (const auto& item : data) {
		CharacterTemplateConfig c;

		c.Id = item.at("Id");
		c.Name = item.at("Name");

		WorldSpawnPos s;
		s = item.at("Pos");

		c.WorldSpawnPos = s;

		c.NumInventorySlot = item.at("NumInventorySlot");
		c.NumBankSlot = item.at("NumBankSlot");

		cTC.push_back(c);
	}

	for (const auto& config : cTC) {
		WorldSpawnPos point = config.WorldSpawnPos;

		point.Roll = archecore::utils::DegToRad(point.Roll);
		point.Pitch = archecore::utils::DegToRad(point.Pitch);
		point.Yaw = archecore::utils::DegToRad(point.Yaw);

		// Males
		auto& cT = m_Templates[(uint8_t)(16 + config.Id)];
		cT.SpawnPosition = point;
		cT.SpawnPosition.WorldId = 0; // WorldManager.DefaultWorld
		cT.NumInventorySlot = config.NumInventorySlot;
		cT.NumBankSlot = config.NumBankSlot;

		// Females
		auto& cTT = m_Templates[(uint8_t)(32 + config.Id)];
		cTT.SpawnPosition = point;
		cTT.SpawnPosition.WorldId = 0; // WorldManager.DefaultWorld
		cTT.NumInventorySlot = config.NumInventorySlot;
		cTT.NumBankSlot = config.NumBankSlot;
	}
	std::cout << std::endl;
}

void CharacterManager::Create(WorldSession& worldSession, std::string& name, uint8_t race, uint8_t gender, uint32_t body[],
	UnitCustomModelData& _customModel, uint8_t ability1) 
{
	using system_clock = std::chrono::system_clock;

	uint8_t nameValidationResult = ValidateCharacterName(name);
	if (nameValidationResult != 0)
	{
		packet p;
		worldSession.SCCharacterCreationFailedPacket(nameValidationResult, p);
		return;
	}

	//characterId = CharacterIdManager.Instance.GetNextId();
	const CharacterTemplate& charTemplate = GetTemplate(race, gender);

	// Todo, get registry(world) for entity
	entt::registry& world = WorldManager::Instance().GetDefaultWorld();

	archecore::utils::StringToNameFormat(name);
	
	worldSession.entityCharacter = WorldManager::Instance().CreateObjectEntity(WorldManager::WORLD_DEFAULT_ID);
	
	Unit& unitComp = world.emplace<Unit>(worldSession.entityCharacter);
	unitComp.Id = (uint32)m_NextCharacterId++;

	unitComp.Name = name;
	unitComp.ModelId = charTemplate.ModelId;
	unitComp.ModelPosture = ModelPostureType::NONE;

	HealthMana& hpMp = world.emplace<HealthMana>(worldSession.entityCharacter);
	hpMp.Hp = 255;
	hpMp.Mp = 255;
	hpMp.MaxHp = 255;
	hpMp.MaxMp = 255;

	Faction& factionComp = world.emplace<Faction>(worldSession.entityCharacter);
	factionComp.FactionId = charTemplate.FactionId;

	Character& charComp = world.emplace<Character>(worldSession.entityCharacter);
	charComp.Race = Race(race);
	charComp.Gender = Gender(gender);
	charComp.AccessLevel = 100;
	charComp.Id = unitComp.Id;	// duplicate from UnitId

	// Temp
	worldSession.characterIds[0] = charComp.Id;

	CharacterInfo& charInfo = world.emplace<CharacterInfo>(worldSession.entityCharacter);
	charInfo.Created = system_clock::to_time_t(system_clock::now());
	charInfo.Updated = system_clock::to_time_t(system_clock::now());
	charInfo.ReturnDistrictId = charTemplate.ReturnDistrictId;
	charInfo.ResurrectionDistrictId = charTemplate.ResurrectionDistrictId;

	world.emplace<CharCurrency>(worldSession.entityCharacter); // all -> 0
	world.emplace<Family>(worldSession.entityCharacter); // all -> 0
	world.emplace<ExpComponent>(worldSession.entityCharacter); // 0

	LaborComponent& laborComp = world.emplace<LaborComponent>(worldSession.entityCharacter);
	laborComp.LaborPower = 50;
	laborComp.LaborPowerModified = system_clock::to_time_t(system_clock::now());

	InventoryBag& bag = world.emplace<InventoryBag>(worldSession.entityCharacter, charTemplate.NumInventorySlot);
	
	InventoryEquipped& invEquipped = world.emplace<InventoryEquipped>(worldSession.entityCharacter);
	invEquipped.Items.fill(entt::null);

	InventoryWarehouse& bank = world.emplace<InventoryWarehouse>(worldSession.entityCharacter, charTemplate.NumBankSlot);

	ActionSlots& actionSlots = world.emplace<ActionSlots>(worldSession.entityCharacter);

	CombatSpec& combatSpec = world.emplace<CombatSpec>(worldSession.entityCharacter);
	combatSpec.Spec1 = ability1;

	AbilityItems& items = m_AbilityItems[ability1];

	CreateDefaultItemSlot(world, invEquipped, items.Items.Headgear, EquipmentItemSlot::Head, items.Items.HeadgearGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Necklace, EquipmentItemSlot::Neck, items.Items.NecklaceGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Shirt, EquipmentItemSlot::Chest, items.Items.ShirtGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Belt, EquipmentItemSlot::Waist, items.Items.BeltGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Pants, EquipmentItemSlot::Legs, items.Items.PantsGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Gloves, EquipmentItemSlot::Hands, items.Items.GlovesGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Shoes, EquipmentItemSlot::Feet, items.Items.ShoesGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Bracelet, EquipmentItemSlot::Arms, items.Items.BraceletGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Back, EquipmentItemSlot::Back, items.Items.BackGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Undershirt, EquipmentItemSlot::Undershirt, items.Items.UndershirtGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Underpants, EquipmentItemSlot::Underpants, items.Items.UnderpantsGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Mainhand, EquipmentItemSlot::Mainhand, items.Items.MainhandGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Offhand, EquipmentItemSlot::Offhand, items.Items.OffhandGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Ranged, EquipmentItemSlot::Ranged, items.Items.RangedGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Instrument, EquipmentItemSlot::Musical, items.Items.InstrumentGrade);
	CreateDefaultItemSlot(world, invEquipped, items.Items.Cosplay, EquipmentItemSlot::Cosplay, items.Items.CosplayGrade);

	for (uint8 i = 0; i < 7; i++)
	{
		if (body[i] == 0 && charTemplate.Items[i] > 0) {
			body[i] = charTemplate.Items[i];
		}
		
		CreateDefaultItemSlot(world, invEquipped, body[i], (EquipmentItemSlot)(i + 19), 0);
	}

	//spdlog::info("Ability Items1...");
	uint8 slot = 10;
	for (auto& item : items.Supplies)
	{
		// Aquire default item...
		ItemManager::Instance().AquireDefaultItem(bag, ItemTaskType::Invalid, item.Id, item.Amount, item.Grade);
		actionSlots.SetAction(slot, ActionSlotType::ItemType, item.Id);
		slot++;
	}

	//spdlog::info("Ability Items2...");
	if (m_AbilityItems.contains(0)) {
		AbilityItems& items2 = m_AbilityItems[0];

		for (auto& item : items2.Supplies) {
			ItemManager::Instance().AquireDefaultItem(bag, ItemTaskType::Invalid, item.Id, item.Amount, item.Grade);
			actionSlots.SetAction(slot, ActionSlotType::ItemType, item.Id);
			slot++;
		}
	}

	CharacterAbilites& charAbilities = world.emplace<CharacterAbilites>(worldSession.entityCharacter);
	charAbilities.SetAbility((AbilityType)combatSpec.Spec1, 0);
	//character->Hp = 255;
	//character->Mp = 255;

	//spdlog::info("Set Character Actability...");
	CharacterActability& charActabilities = world.emplace<CharacterActability>(worldSession.entityCharacter);
	for (auto& [id, actability] : m_Actabilites) {
		charActabilities.Actabilities.insert({ id, Actability(actability.Id) });
		//spdlog::info("Actability inserted: {}", id);
	}

	CharacterSkills& Skills = world.emplace<CharacterSkills>(worldSession.entityCharacter);
	for (const auto& [id, defSkill] : SkillManager::Instance().GetDefaultSkills()) {
		if (defSkill.bAddToSlot == false) { continue; }
		actionSlots.SetAction(defSkill.Slot, ActionSlotType::Spell, defSkill.Prototype.Id);
	}

	/// A bit glonky to do it like this... fix it Mort.. (*kicks mouse lemur forward)
	slot = 1;
	while (actionSlots.Slots[slot].ActionType != ActionSlotType::None) {
		slot++;
	}
	for (const auto& skillId : SkillManager::Instance().GetStartAbilitySkills((AbilityType)ability1)) {
		Skills.AddSkill(skillId, 1, false);
		actionSlots.SetAction(slot, ActionSlotType::Spell, skillId);
		slot++;
	}

	//world.emplace<CharacterAppellations>(worldSession.entityCharacter);
	//world.emplace<CharacterQuests>(worldSession.entityCharacter);
	//world.emplace<CharacterMails>(worldSession.entityCharacter);
	//world.emplace<CharacterPortals>(worldSession.entityCharacter);
	//world.emplace<CharacterFriends>(worldSession.entityCharacter);

	Transform& transform = world.emplace<Transform>(worldSession.entityCharacter);
	transform.Position.x = charTemplate.SpawnPosition.X;
	transform.Position.y = charTemplate.SpawnPosition.Y;
	transform.Position.z = charTemplate.SpawnPosition.Z;
	glm::vec3 spawnRotation = glm::vec3(charTemplate.SpawnPosition.Roll, charTemplate.SpawnPosition.Pitch, charTemplate.SpawnPosition.Yaw);
	transform.SetRotation(spawnRotation);

	transform.ZoneId = charTemplate.ZoneId;

	UnitCustomModelData& customModel = world.emplace<UnitCustomModelData>(worldSession.entityCharacter);
	customModel = std::move(_customModel);
	//customModel.ModelId = charTemplate.ModelId;
	//character->ModelParams = customModel;

	return;
}
uint8_t CharacterManager::ValidateCharacterName(std::string& name)
{
	// Reasons of failure:
		//	1 = This name is pending deletion. Please try again after maintenance.
		//	2 = This name is pending deletion. Please try again after maintenance.
		//	3 = This name is pending deletion. Please try again after maintenance.
		//	4 = taken
		//	5 = illegal chars

	if (archecore::utils::ValidateCustomCharacters(name) == false)
	{
		// Name contains illegal characters...
		return 5;
	}

	// Even if the client checks for length etc.. the user could modify packets in other ways
	if (name.size() < 2 || name.size() > 14)
	{
		return 1; // 
	}

	// TODO: Check database for already taken name
	// if taken, reason = 4

	// Success
	return 0;
}

void CharacterManager::CreateDefaultItemSlot(entt::registry& world, InventoryEquipped& invEquip, uint32 templateId, EquipmentItemSlot slot, uint8 grade)
{
	//std::cout << "Trying to create Item w/ ItemId: " << templateId << std::endl;
	if (templateId > 0) 
	{
		entt::entity item = ItemManager::Instance().CreateItem(world, templateId, 1, grade);
		if (world.valid(item)) {

			invEquip.Items[(uint8)slot] = item;
		}
	}
	
}


