#pragma once
#include "common_archecore.h"
#include "Entity/common_entity.h"
#include "spdlog/spdlog.h"
#include <unordered_map>
#include "Zones/ZoneGroup.h"

struct Zone {
	uint32 Id = 0;
	std::string Name = "";
	uint32 ZoneKey = 0;
	uint32 GroupId = 0;
	bool bClosed = false;
	uint32 FactionId = 0;
	uint32 ZoneClimateId = 0;
};

class ZoneManager {
private:
	ZoneManager(){}
	~ZoneManager(){}
public:
	static ZoneManager& Instance() {
		static ZoneManager _Instance;
		return _Instance;
	}

	void Initialize();

	const std::unordered_map<uint16, ZoneConflict>& GetZoneConflicts() {
		return m_ZoneConflicts;
	}

private:
	std::unordered_map<uint32, Zone> m_Zones;
	std::unordered_map<uint32, uint32> m_ZoneIdToKey;
	std::unordered_map<uint32, ZoneGroup> m_ZoneGroups;
	std::unordered_map<uint16, ZoneConflict> m_ZoneConflicts;
};