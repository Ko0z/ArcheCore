#pragma once

#include "common_archecore.h"
#include "common_utils.h"
#include <sqlite3.h>
#include <string>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace SQLiteUtility {

	inline void SetBool(bool& value, const unsigned char* sqlText) {
		if (sqlText) { value = sqlText[0] == 't' ? true : false; }
	}

	inline void SetString(std::string& str, const unsigned char* sqlText) {
		if (sqlText) {
			str = std::string((const char*)sqlText);
		}
		else {
			str = "";
		}
	}

	inline void SetTime(std::time_t& value, const unsigned char* sqlText) {
		if (sqlText && sqlText[0] != '0') 
		{
			//strptime(sqlText, "%Y-%m-%d %H:%M:%S", &tm);

			std::istringstream ss((const char*)sqlText);
			struct tm tm = {};

			ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
			value = mktime(&tm);
			
			//std::cout << "Set time Nicely: " << std::chrono::system_clock::from_time_t(value) << std::endl;
		}
		else {
			value = archecore::utils::GetTimeMin();
			//std::cout << "Set time min: " << std::chrono::system_clock::from_time_t(value) << std::endl;
		}
		
	}
}
