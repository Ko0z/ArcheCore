#include "SkillManager.h"
#include "Managers/SqLiteManager.h"
#include "Utility/SQLiteUtility.h"
#include "spdlog/spdlog.h"

using namespace e;

void SkillManager::Initialize()
{
	SqLiteManager& sm = SqLiteManager::Instance();
	sqlite3* db = sm.SqlOpen();

	// ------------>
	//spdlog::info("[SkillManager] - Loading Skill Templates...");
	
	const char* command = "SELECT * FROM skills";
	sqlite3_stmt* stmt = sm.SqlPrepare(db, command);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {
		auto sTemplate = SkillTemplate();
		//m_ItemConfig.DeathDurabilityLossRatio = sqlite3_column_int(stmt, 6);
		//m_ItemConfig.DurabilityDecrementChance = (float)sqlite3_column_double(stmt, 1);
		sTemplate.Id = sqlite3_column_int(stmt, 0);
		sTemplate.Cost = sqlite3_column_int(stmt, 3);
		SQLiteUtility::SetBool(sTemplate.Show, sqlite3_column_text(stmt, 5));
		// FireAnim...
		sTemplate.AbilityId = sqlite3_column_int(stmt, 8);
		sTemplate.ManaCost = sqlite3_column_int(stmt, 9);
		sTemplate.TimingId = sqlite3_column_int(stmt, 10);
		sTemplate.CooldownTime = sqlite3_column_int(stmt, 12);
		sTemplate.CastingTime = sqlite3_column_int(stmt, 13);
		SQLiteUtility::SetBool(sTemplate.IgnoreGlobalCooldown, sqlite3_column_text(stmt, 14));
		sTemplate.EffectDelay = sqlite3_column_int(stmt, 15);
		sTemplate.EffectSpeed = (float)sqlite3_column_double(stmt, 16);
		sTemplate.EffectRepeatCount = sqlite3_column_int(stmt, 17);
		sTemplate.EffectRepeatTick = sqlite3_column_int(stmt, 18);
		sTemplate.ActiveWeaponId = sqlite3_column_int(stmt, 20);
		//sTemplate.TargetType = (SkillTargetType)sqlite3_column_int(stmt, 21);
		//sTemplate.TargetSelection = (SkillTargetSelection)sqlite3_column_int(stmt, 21);
		//sTemplate.TargetRelation = (SkillTargetRelation)sqlite3_column_int(stmt, 21);
		sTemplate.TargetAreaCount = sqlite3_column_int(stmt, 24);
		sTemplate.TargetAreaRadius = sqlite3_column_int(stmt, 25);
		SQLiteUtility::SetBool(sTemplate.TargetSiege, sqlite3_column_text(stmt, 26));
		sTemplate.WeaponSlotForAngleId = sqlite3_column_int(stmt, 27);
		sTemplate.TargetAngle = sqlite3_column_int(stmt, 28);
		sTemplate.WeaponSlotForRangeId = sqlite3_column_int(stmt, 29);
		sTemplate.MinRange = sqlite3_column_int(stmt, 30);
		sTemplate.MaxRange = sqlite3_column_int(stmt, 31);
		SQLiteUtility::SetBool(sTemplate.KeepStealth, sqlite3_column_text(stmt, 32));
		sTemplate.Aggro = sqlite3_column_int(stmt, 34);
		sTemplate.ChannelingTime = sqlite3_column_int(stmt, 38);
		sTemplate.ChannelingTick = sqlite3_column_int(stmt, 39);
		sTemplate.ChannelingMana = sqlite3_column_int(stmt, 40);
		sTemplate.ChannelingTargetBuffId = sqlite3_column_int(stmt, 42);
		sTemplate.TargetAreaAngle = sqlite3_column_int(stmt, 43);
		sTemplate.AbilityLevel = sqlite3_column_int(stmt, 44);
		sTemplate.ChannelingDoodadId = sqlite3_column_int(stmt, 45);
		sTemplate.CooldownTagId = sqlite3_column_int(stmt, 46);
		sTemplate.SkillControllerId = sqlite3_column_int(stmt, 47);
		sTemplate.RepeatCount = sqlite3_column_int(stmt, 48);
		sTemplate.RepeatTick = sqlite3_column_int(stmt, 49);
		sTemplate.ToggleBuffId = sqlite3_column_int(stmt, 50);
		SQLiteUtility::SetBool(sTemplate.TargetDead, sqlite3_column_text(stmt, 51));
		sTemplate.ChannelingBuffId = sqlite3_column_int(stmt, 52);
		sTemplate.ReagentCorpseStatusId = sqlite3_column_int(stmt, 53);
		SQLiteUtility::SetBool(sTemplate.SourceDead, sqlite3_column_text(stmt, 54));
		sTemplate.LevelStep = sqlite3_column_int(stmt, 55);
		sTemplate.ValidHeight = (float)sqlite3_column_double(stmt, 56);
		sTemplate.TargetValidHeight = (float)sqlite3_column_double(stmt, 57);
		SQLiteUtility::SetBool(sTemplate.SourceMount, sqlite3_column_text(stmt, 58));
		SQLiteUtility::SetBool(sTemplate.StopCastingOnBigHit, sqlite3_column_text(stmt, 59));
		SQLiteUtility::SetBool(sTemplate.StopChannelingOnBigHit, sqlite3_column_text(stmt, 60));
		SQLiteUtility::SetBool(sTemplate.AutoLearn, sqlite3_column_text(stmt, 61));
		SQLiteUtility::SetBool(sTemplate.NeedLearn, sqlite3_column_text(stmt, 62));
		sTemplate.MainhandToolId = sqlite3_column_int(stmt, 63);
		sTemplate.OffhandToolId = sqlite3_column_int(stmt, 64);
		sTemplate.FrontAngle = sqlite3_column_int(stmt, 65);
		sTemplate.ManaLevelMd = (float)sqlite3_column_double(stmt, 66);
		SQLiteUtility::SetBool(sTemplate.Unmount, sqlite3_column_text(stmt, 68));
		sTemplate.DamageTypeId = sqlite3_column_int(stmt, 69);
		SQLiteUtility::SetBool(sTemplate.AllowToPrisoner, sqlite3_column_text(stmt, 70));
		sTemplate.MilestoneId = sqlite3_column_int(stmt, 71);
		SQLiteUtility::SetBool(sTemplate.MatchAnimation, sqlite3_column_text(stmt, 72));
		// Plot
		SQLiteUtility::SetBool(sTemplate.UseAnimTime, sqlite3_column_text(stmt, 74));
		sTemplate.ConsumeLaborPower = sqlite3_column_int(stmt, 76);
		SQLiteUtility::SetBool(sTemplate.SourceStun, sqlite3_column_text(stmt, 77));
		SQLiteUtility::SetBool(sTemplate.TargetAlive, sqlite3_column_text(stmt, 78));
		SQLiteUtility::SetBool(sTemplate.TargetWater, sqlite3_column_text(stmt, 80));
		sTemplate.CastingInc = sqlite3_column_int(stmt, 90);
		SQLiteUtility::SetBool(sTemplate.CastingCancelable, sqlite3_column_text(stmt, 91));
		SQLiteUtility::SetBool(sTemplate.CastingDelayable, sqlite3_column_text(stmt, 92));
		SQLiteUtility::SetBool(sTemplate.ChannelingCancelable, sqlite3_column_text(stmt, 93));
		sTemplate.TargetOffsetAngle = (float)sqlite3_column_double(stmt, 94);
		sTemplate.TargetOffsetDistance = (float)sqlite3_column_double(stmt, 95);
		sTemplate.ActabilityGroupId = sqlite3_column_int(stmt, 96);
		SQLiteUtility::SetBool(sTemplate.PlotOnly, sqlite3_column_text(stmt, 97));
		SQLiteUtility::SetBool(sTemplate.SkillControllerAtEnd, sqlite3_column_text(stmt, 99));
		SQLiteUtility::SetBool(sTemplate.EndSkillController, sqlite3_column_text(stmt, 100));
		SQLiteUtility::SetBool(sTemplate.OrUnitReqs, sqlite3_column_text(stmt, 107));
		SQLiteUtility::SetBool(sTemplate.DefaultGcd, sqlite3_column_text(stmt, 108));
		SQLiteUtility::SetBool(sTemplate.KeepManaRegen, sqlite3_column_text(stmt, 113));
		sTemplate.CrimePoint = sqlite3_column_int(stmt, 114);
		SQLiteUtility::SetBool(sTemplate.LevelRuleNoConsideration, sqlite3_column_text(stmt, 115));
		SQLiteUtility::SetBool(sTemplate.UseWeaponCooldownTime, sqlite3_column_text(stmt, 116));
		sTemplate.CombatDiceId = sqlite3_column_int(stmt, 121);
		sTemplate.CustomGcd = sqlite3_column_int(stmt, 123);
		SQLiteUtility::SetBool(sTemplate.CancelOngoingBuffs, sqlite3_column_text(stmt, 124));
		sTemplate.CancelOngoingBuffExceptionTagId = sqlite3_column_int(stmt, 125);
		SQLiteUtility::SetBool(sTemplate.SourceCannotUseWhileWalk, sqlite3_column_text(stmt, 126));
		SQLiteUtility::SetBool(sTemplate.SourceMountMate, sqlite3_column_text(stmt, 127));
		SQLiteUtility::SetBool(sTemplate.CheckTerrain, sqlite3_column_text(stmt, 131));
		SQLiteUtility::SetBool(sTemplate.TargetOnlyWater, sqlite3_column_text(stmt, 132));
		SQLiteUtility::SetBool(sTemplate.SourceNotSwim, sqlite3_column_text(stmt, 133));
		SQLiteUtility::SetBool(sTemplate.TargetPreoccupied, sqlite3_column_text(stmt, 134));
		SQLiteUtility::SetBool(sTemplate.StopChannelingOnStartSkill, sqlite3_column_text(stmt, 135));
		SQLiteUtility::SetBool(sTemplate.StopCastingByTurn, sqlite3_column_text(stmt, 136));
		SQLiteUtility::SetBool(sTemplate.TargetMyNpc, sqlite3_column_text(stmt, 137));
		sTemplate.GainLifePoint = sqlite3_column_int(stmt, 138);
		SQLiteUtility::SetBool(sTemplate.TargetFishing, sqlite3_column_text(stmt, 139));
		SQLiteUtility::SetBool(sTemplate.SourceNoSlave, sqlite3_column_text(stmt, 140));
		SQLiteUtility::SetBool(sTemplate.AutoReUse, sqlite3_column_text(stmt, 141));
		sTemplate.AutoReUseDelay = sqlite3_column_int(stmt, 142);
		SQLiteUtility::SetBool(sTemplate.SourceNotCollided, sqlite3_column_text(stmt, 143));
		sTemplate.SkillPoints = sqlite3_column_int(stmt, 144);
		sTemplate.DoodadHitFamily = sqlite3_column_int(stmt, 145);

		m_SkillTemplates.insert({ sTemplate.Id, sTemplate });
	} sqlite3_finalize(stmt);

	//std::cout << "Debug Test" << std::endl << std::endl;
	spdlog::info("[SkillManager] - Loaded Skill Templates: {}", m_SkillTemplates.size());
	// Debug seems correct
	//for (auto& prot : m_SkillTemplates) {
	//	if (prot.first > 10000 && prot.first < 10200) {
	//		std::cout << "Debug Test" << std::endl << std::endl;
	//	}
	//}

	//spdlog::info("[SkillManager] - Loading Default Skills...");

	const char* command2 = "SELECT * FROM default_skills";
	stmt = sm.SqlPrepare(db, command2);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {

		uint32 id = sqlite3_column_int(stmt, 1);

		//if (!m_SkillTemplates.contains(id)) {
		//	spdlog::error("DefaultSkill: missing template for skillId");
		//}

		DefaultSkill defaultSkill{ m_SkillTemplates[id] };

		defaultSkill.Slot = sqlite3_column_int(stmt, 2);
		SQLiteUtility::SetBool(defaultSkill.bAddToSlot, sqlite3_column_text(stmt, 3));
		
		m_DefaultSkills.insert({ defaultSkill.Prototype.Id, defaultSkill });
	} sqlite3_finalize(stmt);

	spdlog::info("[SkillManager] - Loaded Default Skills: {}", m_DefaultSkills.size());


	sqlite3_close(db);
	InitializeHelpContainers();
}

void SkillManager::InitializeHelpContainers()
{
	/// Sorry, but I hate that template is a reserved keyword in c++, t = template
	for (const auto& [id, t] : m_SkillTemplates) 
	{
		if (!t.AutoLearn)
			continue;

		if (!t.NeedLearn && t.AbilityId == 0 && !m_DefaultSkills.contains(t.AbilityId))
			m_CommonSkills.push_back(id);
		
		if (!t.NeedLearn && t.AbilityId == 0 || t.AbilityLevel > 1 || !t.Show)
			continue;
		
		auto abilityType = (AbilityType)t.AbilityId;
		m_StartAbilitySkills[abilityType].push_back(id);
	}

	// Debug stuff, ignore ---->
	//for (size_t i = 0; i < m_StartAbilitySkills.size(); i++)
	//{
	//	spdlog::info("Found StartAbilitySkills for type {} : {}", i, m_StartAbilitySkills[(AbilityType)i].size());
	//	
	//}
	spdlog::info("[SkillManager] - Found DefaultSkills: {}", m_DefaultSkills.size());
	spdlog::info("[SkillManager] - Found CommonSkills: {}", m_CommonSkills.size());
}
