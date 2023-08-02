#include "FactionManager.h"
#include "Utility/SQLiteUtility.h"
#include "SqLiteManager.h"
#include "Managers/LocalizationManager.h"
#include <spdlog/spdlog.h>

void FactionManager::Initialize()
{
	if (s_IsInitialized == true || LoadFactionsFromDatabase() == false)
	{
		spdlog::error("[FactionManager][ERROR] - Failed to initialize FactionManager");
		return;
	}
	//spdlog::info("[FactionManager] - SystemFaction loaded successfully");
}

/// Not used yet
/*
void FactionManager::SendFactions(WorldSession& worldSession)
{
	size_t numValues = s_SystemFactions.size();

	if (numValues == 0)
	{
		std::cerr << "Error, s_SystemFactions is empty" << std::endl;
	}
	else
	{
		std::vector<SystemFaction> factions;
		factions.reserve(numValues);

		for (const auto& [key, value] : s_SystemFactions)
		{
			factions.push_back(value);
		}

		worldSession.SCFactionListPacket(factions);
	}
}

void FactionManager::SendRelations(WorldSession& worldSession)
{
	size_t numRelations = s_Relations.size();

	if (numRelations == 0)
	{
		std::cerr << "Error, s_SystemFactions is empty" << std::endl;
		return;
	}

	worldSession.SCFactionRelationListPacket(s_Relations);
}
*/
SystemFaction* FactionManager::GetFaction(uint32_t id)
{
	if (s_SystemFactions.find(id) != s_SystemFactions.end())
	{
		return &s_SystemFactions[id];
	}
	return nullptr;
}

bool FactionManager::LoadFactionsFromDatabase()
{
	// Read from SQL all factions and load them in to the static map
	//sqlite3* db = SqlOpen();

	SqLiteManager& sm = SqLiteManager::Instance();
	sqlite3* db = sm.SqlOpen();


	/* System Factions */
	const char* command = "SELECT * FROM system_factions";
	sqlite3_stmt* stmt = sm.SqlPrepare(db, command);

	//spdlog::info("[FactionManager] - Loading System Factions...");

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) 
	{
		SystemFaction sF;

		sF.m_Id = sqlite3_column_int(stmt, 0);
		LocalizationManager::Instance().SetFactionTranslation(sF.m_Id, sF.m_Name);
		if (sF.m_Name == "") {
			SQLiteUtility::SetString(sF.m_Name, sqlite3_column_text(stmt, 1));
		}
		
		SQLiteUtility::SetString(sF.m_OwnerName, sqlite3_column_text(stmt, 2));

		sF.m_UnitOwnerType = sqlite3_column_int(stmt, 3);
		sF.m_OwnerId = sqlite3_column_int(stmt, 4);
		sF.m_PoliticalSystem = sqlite3_column_int(stmt, 5);
		sF.m_MotherId = sqlite3_column_int(stmt, 6);
		SQLiteUtility::SetBool(sF.m_AggroLink, sqlite3_column_text(stmt, 7));
		SQLiteUtility::SetBool(sF.m_GuardHelp, sqlite3_column_text(stmt, 8));
		SQLiteUtility::SetBool(sF.m_DiplomacyTarget, sqlite3_column_text(stmt, 9));
		
		s_SystemFactions.insert({ sF.m_Id, sF });
	} sqlite3_finalize(stmt);
	
	spdlog::info("[FactionManager] - Loaded System Factions: {}", s_SystemFactions.size());
	// ----------------->

	command = "SELECT * FROM system_faction_relations";
	stmt = sm.SqlPrepare(db, command);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) 
	{
		int id1 = sqlite3_column_int(stmt, 1);
		int id2 = sqlite3_column_int(stmt, 2);
		int stateId = sqlite3_column_int(stmt, 3);

		FactionRelation fR;
		fR.id = id1;
		fR.id2 = id2;
		fR.state = RelationState(stateId);
		s_Relations.push_back(fR);

		SystemFaction faction = s_SystemFactions[fR.id];
		faction.m_RelationsMap.insert({ fR.id2, fR });
		faction = s_SystemFactions[fR.id2];
		faction.m_RelationsMap.insert({ fR.id, fR });

	} sqlite3_finalize(stmt);
	
	spdlog::info("[FactionManager] - Loaded Faction Relations: {}", s_Relations.size());
	
	sqlite3_close(db);
	s_IsInitialized = true;

	return true;
}