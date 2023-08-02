#pragma once
#include "Faction/SystemFaction.h"
#include "common_archecore.h"
#include <map>
#include <vector>

class WorldSession;

class FactionManager
{
private:
	FactionManager() {}

public:
	static FactionManager& Instance() {
		static FactionManager _Instance;
		return _Instance;
	}

	SystemFaction* GetFaction(uint32_t id);
	
	std::vector<FactionRelation>& GetRelations() { return s_Relations; }
	std::map<uint32_t, SystemFaction>& GetFactions() { return s_SystemFactions; }

	// Should only be called once at the start 
	void Initialize();
	
	/// Not used yet
	//void SendFactions(WorldSession& worldSession);
	//
	//void SendRelations(WorldSession& worldSession);
	
private:
	bool LoadFactionsFromDatabase();

private:
	bool s_IsInitialized;
	std::map<uint32_t, SystemFaction> s_SystemFactions;
	std::vector<FactionRelation> s_Relations;
};