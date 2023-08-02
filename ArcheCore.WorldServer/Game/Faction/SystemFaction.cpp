#include "SystemFaction.h"
#include "Managers/FactionManager.h"

RelationState SystemFaction::GetRelationState(SystemFaction& otherFaction)
{
	uint32_t factionId = m_MotherId != 0 ? m_MotherId : m_Id;
	uint32_t otherFactionId = otherFaction.m_MotherId != 0 ? otherFaction.m_MotherId : otherFaction.m_Id;

	if (factionId == otherFactionId)
		return RelationState::RELATIONSTATE_FRIENDLY;

	if (m_MotherId != 0)
	{
		//SystemFaction* pSystemFaction = FactionManager::GetFaction(m_MotherId);
		SystemFaction* pSystemFaction = FactionManager::Instance().GetFaction(m_MotherId);
		if (pSystemFaction != nullptr)
		{
			auto& relation = pSystemFaction->m_RelationsMap;
			auto it = relation.find(otherFactionId);
			if (it != relation.end())
				return it->second.state;
		}
	}

	auto it = m_RelationsMap.find(otherFactionId);
	if (it != m_RelationsMap.end())
	{
		return it->second.state;
	}

	return RelationState::RELATIONSTATE_NEUTRAL;
}