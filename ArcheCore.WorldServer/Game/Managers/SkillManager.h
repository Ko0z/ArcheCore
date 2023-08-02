#pragma once
#include "common_archecore.h"
#include "Entity/common_entity.h"
#include "spdlog/spdlog.h"
#include <unordered_map>

/// <summary>
/// Instead of having a manager do this, maybe have static memebers in the regular Skill struct?
/// </summary>
class SkillManager {
private:
	SkillManager() {}
	~SkillManager() {}

public:
	static SkillManager& Instance() {
		static SkillManager _Instance;
		return _Instance;
	}

	void Initialize();

	const e::SkillTemplate* GetSkillTemplate(uint32 id)
	{
		if (m_SkillTemplates.contains(id)) {
			return &m_SkillTemplates[id];
		}
		else {
			spdlog::error("[SkillManager] - Did not find template id: {0:d}", id);
		}

		return nullptr;
	}

	const std::unordered_map<uint32, e::DefaultSkill>& GetDefaultSkills() {
		return m_DefaultSkills;
	}

	const std::vector<uint32>& GetStartAbilitySkills(e::AbilityType forType) {
		return m_StartAbilitySkills[forType];
	}

private:
	void InitializeHelpContainers();

private:
	// Temp, store templates and prototypes on memory for now, read directly from db later...
	std::unordered_map<uint32, e::SkillTemplate> m_SkillTemplates;
	std::unordered_map<uint32, e::DefaultSkill> m_DefaultSkills;

	std::unordered_map<e::AbilityType, std::vector<uint32>> m_StartAbilitySkills;
	std::vector<uint32> m_CommonSkills;

};