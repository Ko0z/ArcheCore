#include "LocalizationManager.h"
#include "Utility/SQLiteUtility.h"
#include "SqLiteManager.h"
#include <spdlog/spdlog.h>

void LocalizationManager::Initialize()
{
	SqLiteManager& sm = SqLiteManager::Instance();
	sqlite3* db = sm.SqlOpen();

	std::string command = "SELECT * FROM localized_texts WHERE tbl_name = 'system_factions'";
	sqlite3_stmt* stmt = sm.SqlPrepare(db, command.c_str());
	
	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {

		std::string translation;
		uint32 idx = sqlite3_column_int(stmt, 3);
		SQLiteUtility::SetString(translation, sqlite3_column_text(stmt, 6));

		m_Translations.insert({ idx, translation });
	} sqlite3_finalize(stmt);

	spdlog::info("[LocalizationManager] - Loaded faction name translations: {}", m_Translations.size());

	command = "SELECT * FROM localized_texts WHERE tbl_name = 'system_factions'";
	stmt = sm.SqlPrepare(db, command.c_str());

	while (int ret_code = sqlite3_step(stmt) == SQLITE_ROW) {

		std::string translation;
		uint32 idx = sqlite3_column_int(stmt, 3);
		SQLiteUtility::SetString(translation, sqlite3_column_text(stmt, 6));

		m_Translations.insert({ idx, translation });
	} sqlite3_finalize(stmt);

	sqlite3_close(db);
}
