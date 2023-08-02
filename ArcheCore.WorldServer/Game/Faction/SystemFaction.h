#pragma once
#include "common_archecore.h"
#include "packet.h"
#include "Character/CharacterStructs.h"
#include <map>

class SystemFaction
{
public:
	RelationState GetRelationState(SystemFaction& otherFaction);
	
	void Write(packet& p)
	{
		p << m_Id << m_AggroLink << m_MotherId << m_Name.c_str() << m_OwnerId << m_OwnerName.c_str() << m_UnitOwnerType
			<< m_PoliticalSystem << m_TimeCreated << m_DiplomacyTarget << m_AllowChangeName;
	}

public:							// Goes into packet
	uint32_t m_Id = 0;				// > 4 
	std::string m_Name = "";			// > ?
	uint32_t m_OwnerId = 0;			// > 4
	std::string m_OwnerName = "";	// > ?
	uint32_t m_MotherId = 0;		// > 4
	int8_t m_UnitOwnerType = 0;		// > 1
	uint8_t m_PoliticalSystem = 0;	// > 1
	bool m_DiplomacyTarget = false;		// > 1
	bool m_AggroLink = false;			// > 1
	bool m_GuardHelp = false;
	uint8_t m_AllowChangeName = 0;	// > 1
	int64_t m_TimeCreated = 0;		// > 8

	std::map<uint32_t, FactionRelation> m_RelationsMap;
};

