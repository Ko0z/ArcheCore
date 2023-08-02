#pragma once
#include "common_archecore.h"
#include "common_entity.h"
#include <unordered_map>

struct Skill;

enum class EBuffEffectState : uint8 {
	CREATED,
	ACTIVE,
	FINISHING,
	FINISHED
};

/// <summary>
/// A buff is a temporary object that is created when a unit should receive a new buff or debuff of a specific ID and possibly type.
/// A buff is not a skill but the effect of a buff is in fact a skill
/// </summary>
struct Buff {


	uint32 SkillId = 0;
	int32 Duration = 0;
	EBuffEffectState BuffState = EBuffEffectState::CREATED;
};

/// <summary>
/// The buff component should be put on all entities that could have buffs, debuffs of special time bound effects
/// </summary>
struct BuffComponent {

	std::unordered_map<uint32, Buff> Buffs;
};