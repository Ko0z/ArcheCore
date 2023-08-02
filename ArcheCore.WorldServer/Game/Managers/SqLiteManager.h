#pragma once
#include <sqlite3.h>
#include <filesystem>

class SqLiteManager {
private:
	SqLiteManager() {}

public:
	static SqLiteManager& Instance() {
		static SqLiteManager _Instance;
		return _Instance;
	}

	/// <summary>
	/// Make sure that the database exist in the correct folder and store the path
	/// </summary>
	/// <returns>success</returns>
	bool Initialize() {
		
		if (m_Initialized == false) {
			return StoreDatabasePath();
		}
		
		return m_Initialized;
	}

	sqlite3_stmt* SqlPrepare(sqlite3* db, const char* sqlCommand) {
		sqlite3_stmt* stmt;
		if (sqlite3_prepare_v2(db, sqlCommand, -1, &stmt, NULL) != SQLITE_OK) {
			std::cerr << "ERROR: while compiling sql: " << sqlite3_errmsg(db) << std::endl;
			sqlite3_close(db);
			sqlite3_finalize(stmt);
		}
		return stmt;
	}

	sqlite3* SqlOpen() {
		sqlite3* db;

		//std::cout << "Trying to connect to SQLite DB at: " << m_DatabasePath << std::endl;

		if (sqlite3_open(m_DatabasePath.c_str(), &db) != SQLITE_OK)
		{
			std::cerr << "ERROR: can't open database: " << sqlite3_errmsg(db) << std::endl;
			sqlite3_close(db);
		}
		return db;
	}

	std::string GetFolderPath() {
		return m_ProjectPath;
	}

private:
	bool StoreDatabasePath() 
	{
		std::filesystem::path path = std::filesystem::current_path();
		std::filesystem::path full_path = std::filesystem::absolute(path);
		// Will make the path string to platform specific format?
		std::filesystem::path preferred_path = full_path.make_preferred();
		std::string real_path_string = preferred_path.string();
		real_path_string = real_path_string + "\\";
		std::replace(real_path_string.begin(), real_path_string.end(), '\\', '/');
		std::string sqlite_string = "data/compact.sqlite3";
		std::string full_sqlite_path_string = real_path_string + sqlite_string;

		m_ProjectPath = real_path_string;
		m_DatabasePath = full_sqlite_path_string;

		std::cout << std::endl << "Exe Path: " << m_ProjectPath << std::endl;

		std::filesystem::path full_sqlite_path = full_sqlite_path_string;

		if (std::filesystem::exists(full_sqlite_path)) {
			std::cout << "SQLite Database Path: " << m_DatabasePath << std::endl << std::endl;
			m_Initialized = true;
			return true;
		}
		else {
			std::cout << std::endl 
				<< "[ERROR][SQLiteManager] - compact.sqlite3 not found. "
				<< "You have to manually copy the 'data' folder to the Build Root Path." << std::endl
				<< "If this is a new clone, you have to get hold of the compact.sqlite3 db and manually place it in 'data' folder."
				<< std::endl << std::endl;
			std::cin.get();
			return false;
		}

	}

private:
	bool m_Initialized = false;
	std::string m_DatabasePath = "nullptr";
	std::string m_ProjectPath = "nullptr";
};