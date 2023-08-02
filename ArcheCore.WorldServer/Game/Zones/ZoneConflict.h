#pragma once
#include "common_archecore.h"

enum class ZoneConflictType : uint8
{
	Tension = 0,
	Danger = 1,
	Dispute = 2,
	Unrest = 3,
	Crisis = 4,
	Conflict = 5,
	War = 6,
	Peace = 7
};

struct ZoneConflict {
	//ZoneGroup owner;

	uint16 ZoneGroupId = 0;
	std::array<uint32, 5> NumKills;
	std::array<uint32, 5> NoKillMin;

	int32 ConflictMin = 0;
	int32 WarMin = 0;
	int32 PeaceMin = 0;

	uint32 PeaceProtectedFactionId = 0;
	uint32 NuiaReturnPointId = 0;
	uint32 HariharaReturnPointId = 0;
	uint32 WarTowerDefId = 0;

	bool bClosed = false;

	time_t NextStateTime = 0;
	uint32 KillCount = 0;

	ZoneConflictType CurrentZoneState = ZoneConflictType(7);
};