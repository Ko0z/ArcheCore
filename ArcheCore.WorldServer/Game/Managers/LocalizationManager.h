#pragma once
#include "common_archecore.h"
#include <unordered_map>
#include <string>

class LocalizationManager {
private:
	LocalizationManager(){}
	~LocalizationManager(){}
public:
	static LocalizationManager& Instance() {
		static LocalizationManager _Instance;
		return _Instance;
	}

	void Initialize();

	void SetFactionTranslation(uint32 id, std::string& result) {
		if (m_Translations.contains(id) && m_Translations[id] != "") {
			result = m_Translations[id];
		}
	}

	std::unordered_map<uint32, std::string> m_Translations;
};