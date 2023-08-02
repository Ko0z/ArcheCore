#include "Skill.h"
#include "spdlog/spdlog.h"
#include "Managers/SkillManager.h"

namespace e 
{

	const SkillTemplate* Skill::GetSkillTemplate(uint32 id) const
	{
		return SkillManager::Instance().GetSkillTemplate(id);
	}

}
