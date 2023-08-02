#pragma once
#include "common_archecore.h"
#include "ZoneConflict.h"

struct ZoneGroup {
	uint32 Id = 0;
	std::string Name = "";
	float X = 0;
	float Y = 0;
	float Width = 0;
	float Height = 0;
	uint32 TargetId = 0;
	uint32 FactionChatRegionId = 0;
	bool bPirateDesperado = false;
	uint32 FishingSeaLootPackId = 0;
	uint32 FishingLandLootPackId = 0;

	uint32 BuffId = 0; // TODO: 1.2 BuffId

	ZoneConflict Conflict;
};