#pragma once
#include "common_archecore.h"
#include "common_entity.h"
#include <unordered_map>

namespace e 
{
	struct SkillTemplate {
		uint32 Id = 0;
		int32 Cost;
		bool Show;
		//Anim FireAnim;
		uint8 AbilityId;
		int32 ManaCost;
		int32 TimingId;
		int32 CooldownTime;
		int32 CastingTime;
		bool IgnoreGlobalCooldown;
		int32 EffectDelay;
		float EffectSpeed;
		int32 EffectRepeatCount;
		int32 EffectRepeatTick;
		int32 ActiveWeaponId;
		//SkillTargetType TargetType;
		//SkillTargetSelection TargetSelection;
		//SkillTargetRelation TargetRelation;
		int32 TargetAreaCount;
		int32 TargetAreaRadius;
		bool TargetSiege;
		int32 WeaponSlotForAngleId;
		int32 TargetAngle;
		int32 WeaponSlotForRangeId;
		int32 MinRange;
		int32 MaxRange;
		bool KeepStealth;
		int32 Aggro;
		int32 ChannelingTime;
		int32 ChannelingTick;
		int32 ChannelingMana;
		uint32 ChannelingTargetBuffId;
		int32 TargetAreaAngle;
		int32 AbilityLevel;
		uint32 ChannelingDoodadId;
		int32 CooldownTagId;
		uint32 SkillControllerId;
		int32 RepeatCount;
		int32 RepeatTick;
		uint32 ToggleBuffId;
		bool TargetDead;
		uint32 ChannelingBuffId;
		int32 ReagentCorpseStatusId;
		bool SourceDead;
		int32 LevelStep;
		float ValidHeight;
		float TargetValidHeight;
		bool SourceMount;
		bool StopCastingOnBigHit;
		bool StopChannelingOnBigHit;
		bool AutoLearn;
		bool NeedLearn;
		uint32 MainhandToolId;
		uint32 OffhandToolId;
		int32 FrontAngle;
		float ManaLevelMd;
		bool Unmount;
		uint32 DamageTypeId;
		bool AllowToPrisoner;
		uint32 MilestoneId;
		bool MatchAnimation;
		//Plot Plot;
		bool UseAnimTime;
		int32 ConsumeLaborPower;
		bool SourceStun;
		bool TargetAlive;
		bool TargetWater;
		int32 CastingInc;
		bool CastingCancelable;
		bool CastingDelayable;
		bool ChannelingCancelable;
		float TargetOffsetAngle;
		float TargetOffsetDistance;
		int32 ActabilityGroupId;
		bool PlotOnly;
		bool SkillControllerAtEnd;
		bool EndSkillController;
		bool OrUnitReqs;
		bool DefaultGcd;
		bool KeepManaRegen;
		int32 CrimePoint;
		bool LevelRuleNoConsideration;
		bool UseWeaponCooldownTime;
		int32 CombatDiceId;
		int32 CustomGcd;
		bool CancelOngoingBuffs;
		uint32 CancelOngoingBuffExceptionTagId;
		bool SourceCannotUseWhileWalk;
		bool SourceMountMate;
		bool CheckTerrain;
		bool TargetOnlyWater;
		bool SourceNotSwim;
		bool TargetPreoccupied;
		bool StopChannelingOnStartSkill;
		bool StopCastingByTurn;
		bool TargetMyNpc;
		int32 GainLifePoint;
		bool TargetFishing;
		bool SourceNoSlave;
		bool AutoReUse;
		int32 AutoReUseDelay;
		bool SourceNotCollided;
		int32 SkillPoints;
		int32 DoodadHitFamily;
		//List<SkillEffect> Effects
	};

	struct DefaultSkill {
		SkillTemplate& Prototype;
		uint8 Slot = 0;
		bool bAddToSlot = false;
	};

	struct Skill {
		
		inline const SkillTemplate* GetSkillTemplate(uint32 id) const;

		/* Members */
		uint32 Id = 0;
		uint8 Level = 0;
		uint16 T_Id = 0; // ?
		// PlotState ActivePlotState;
		bool bIgnoreGCD = false;
		bool bCancelled = false;
		// Action Callback...
		std::unordered_map<uint8, SkillHitType> HitTypes;	// surely there must be a better option, like mask, bitmask...
	};

	struct PassiveBuff {
		PassiveBuff(uint32 id = 0) {
			Id = id;
		}

		void Apply(entt::entity owner) {
			//auto buffTemplate = SkillManager::Instance().GetBuffTemplate(Id);
			// new buff()...
			//owner->Buffs.Add(buff)...
		}

		void Remove(entt::entity owner) {
			//owner->Buffs.Remove(buff)...
		}

		uint32 Id = 0;
	};
}
