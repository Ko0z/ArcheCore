#include "ZoneManager.h"
#include "Utility/SQLiteUtility.h"
#include "SqLiteManager.h"

#include <spdlog/spdlog.h>

void ZoneManager::Initialize()
{
	SqLiteManager& sm = SqLiteManager::Instance();
	sqlite3* db = sm.SqlOpen();

	//spdlog::info("Loading Zones...");

	const char* command = "SELECT * FROM zones";
	sqlite3_stmt* stmt = sm.SqlPrepare(db, command);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		Zone zone;

		zone.Id = sqlite3_column_int(stmt, 0);
		SQLiteUtility::SetString(zone.Name, sqlite3_column_text(stmt, 1));
		zone.ZoneKey = sqlite3_column_int(stmt, 2);
		zone.GroupId = sqlite3_column_int(stmt, 3);
		SQLiteUtility::SetBool(zone.bClosed, sqlite3_column_text(stmt, 4));
		zone.FactionId = sqlite3_column_int(stmt, 6);
		zone.ZoneClimateId = sqlite3_column_int(stmt, 7);

		m_Zones.insert({ zone.ZoneKey, zone });
	} sqlite3_finalize(stmt);

	spdlog::info("[ZoneManager] - Loaded Zones: {}", m_Zones.size());

	const char* command2 = "SELECT * FROM zone_groups";
	stmt = sm.SqlPrepare(db, command2);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		ZoneGroup zoneGroup;

		zoneGroup.Id = sqlite3_column_int(stmt, 0);
		SQLiteUtility::SetString(zoneGroup.Name, sqlite3_column_text(stmt, 1));
		zoneGroup.X = (float)sqlite3_column_double(stmt, 2);
		zoneGroup.Y = (float)sqlite3_column_double(stmt, 3);
		zoneGroup.Width = (float)sqlite3_column_double(stmt, 4);
		zoneGroup.Height = (float)sqlite3_column_double(stmt, 5);
		zoneGroup.TargetId = sqlite3_column_int(stmt, 8);
		zoneGroup.FactionChatRegionId = sqlite3_column_int(stmt, 10);
		SQLiteUtility::SetBool(zoneGroup.bPirateDesperado, sqlite3_column_text(stmt, 12));
		zoneGroup.FishingSeaLootPackId = sqlite3_column_int(stmt, 13);
		zoneGroup.FishingLandLootPackId = sqlite3_column_int(stmt, 14);
		zoneGroup.BuffId = sqlite3_column_int(stmt, 15);

		m_ZoneGroups.insert({ zoneGroup.Id, zoneGroup });
	} sqlite3_finalize(stmt);

	spdlog::info("[ZoneManager] - Loaded ZoneGroups: {}", m_ZoneGroups.size());

	const char* command3 = "SELECT * FROM conflict_zones";
	stmt = sm.SqlPrepare(db, command3);

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW)
	{
		uint16 zoneGroupId = sqlite3_column_int(stmt, 0);

		if (!m_ZoneGroups.contains(zoneGroupId)) continue;

		ZoneConflict zoneConflict;
		zoneConflict.ZoneGroupId = zoneGroupId;

		for (uint8 i = 1; i < 6; i++)
		{
			zoneConflict.NumKills[i - 1] = sqlite3_column_int(stmt, i);
		}
		for (uint8 i = 6; i < 11; i++)
		{
			zoneConflict.NoKillMin[i - 6] = sqlite3_column_int(stmt, i);
		}
		zoneConflict.ConflictMin = sqlite3_column_int(stmt, 11);
		zoneConflict.WarMin = sqlite3_column_int(stmt, 12);
		zoneConflict.PeaceMin = sqlite3_column_int(stmt, 13);

		zoneConflict.PeaceProtectedFactionId = sqlite3_column_int(stmt, 14);
		zoneConflict.NuiaReturnPointId = sqlite3_column_int(stmt, 15);
		zoneConflict.HariharaReturnPointId = sqlite3_column_int(stmt, 16);
		zoneConflict.WarTowerDefId = sqlite3_column_int(stmt, 17);

		SQLiteUtility::SetBool(zoneConflict.bClosed, sqlite3_column_text(stmt, 19));

		m_ZoneGroups[zoneGroupId].Conflict = zoneConflict;
		m_ZoneConflicts.insert({ zoneGroupId, zoneConflict });

		
	} sqlite3_finalize(stmt);

	spdlog::info("[ZoneManager] - Loaded ConflictZones: {}", m_ZoneConflicts.size());
	spdlog::warn("[ZoneManager] - There is more data to be loaded later...");

	sqlite3_close(db);
}
