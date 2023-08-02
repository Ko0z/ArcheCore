#pragma once
#include "common_archecore.h"
#include "common_entity.h"
#include "packet.h"
#include <unordered_map>

//struct packet;
struct PassiveBuff;
struct Skill;

namespace e {

	struct Character {

		void Write(packet& p, entt::registry& world, entt::entity entityCharacter) const;

		uint8 GetRaceGender() {
			return (uint8)(16 * (uint8)Gender + (uint8)Race);
		}

		Race Race = e::Race::None;
		Gender Gender = e::Gender::Male;
		int32 AccessLevel = 0;
		uint32 Id = 0; // Unique Character Id
	};

	///----------[ Character specific ]-------------------->

	/// <summary>
	/// This is really only useful for ZoneId at the moment
	/// </summary>
	//struct WorldSpawnCharacter {
	//	float X = 0;
	//	float Y = 0;
	//	float Z = 0;
	//	float Roll = 0;
	//	float Pitch = 0;
	//	float Yaw = 0;
	//	uint32_t WorldId = 1;
	//	uint32_t ZoneId = 0;
	//};

	struct CharacterInfo {
		time_t Created = 0;
		time_t Updated = 0;
		uint32 ReturnDistrictId = 0;
		uint32 ResurrectionDistrictId = 0;
		int16 DeadCount = 0;
		time_t DeadTime = 0;
		int32 RezWaitDuration = 0;
		time_t RezTime = 0;
		int32 RezPenaltyDuration = 0;
		time_t LeaveTime = 0;	// Last world leave time
		time_t DeleteRequestTime = 0;
		time_t TransferRequestTime = 0;
		time_t DeleteTime = 0;
		bool bUseAAPoint = false;
		int32 PrevPoint = 0;
		int32 Point = 0;
		int32 Gift = 0;
	};

	/// <summary>
	/// The three combat specializations that make up a class like Darkrunner, Spellsinger etc...
	/// </summary>
	struct CombatSpec {

		uint8 GetNumActive() 
		{
			uint8 sum = 0;
			if (Spec1 != (uint8)AbilityType::None)
				sum++;
			if (Spec2 != (uint8)AbilityType::None)
				sum++;
			if (Spec3 != (uint8)AbilityType::None)
				sum++;

			return sum;
		}

		void Write(packet& p) 
		{
			if (Spec1 != (uint8)AbilityType::None)
				p << Spec1;
			if (Spec2 != (uint8)AbilityType::None)
				p << Spec2;
			if (Spec3 != (uint8)AbilityType::None)
				p << Spec3;
		}

		uint8 Spec1 = 0;
		uint8 Spec2 = 11;
		uint8 Spec3 = 11;
	};

	struct Family {
		uint32 FamilyId = 0;
	};

	// Ability1
	// Ability2
	// Ability3

	struct LaborComponent {
		int16 LaborPower = 0;
		int32 ConsumedLaborPower = 0;
		time_t LaborPowerModified = 0;
	};

	struct CharCurrency {
		int64 Money = 0;
		int64 MoneyBank = 0;
		int32 HonorPoints = 0;
		int32 VocationPoints = 0;
		int64 LoyaltyTokens = 0;
		int16 CrimePoints = 0;	// Current crime points ( /50)
		int32 CrimeRecord = 0; // Total infamy
	};

	struct ActionSlot {
		ActionSlotType ActionType = ActionSlotType::None;
		uint64 ActionId = 0;
	};

	struct ActionSlots {

		void SetAction(uint8 slot, ActionSlotType type, uint32 actionId);

		std::array<ActionSlot, 85> Slots;
	};

	struct Ability {
		Ability(AbilityType id = AbilityType::None) {
			Id = id;
		}

		AbilityType Id;
		uint8 Order = 255;
		int32 Exp = 0;
	};

	// Combat Specializations, Classes, Spec, Talents... whatever we should call them
	struct CharacterAbilites {
		CharacterAbilites() {
			for (uint8 i = 0; i < 11; i++)
			{
				auto id = (AbilityType)i;
				Abilities[id] = Ability(id);
			}
		}

		void Write(packet& p) {
			for (uint8 i = 0; i < 11; i++)
			{
				p << Abilities[(AbilityType)i].Exp;
				p << Abilities[(AbilityType)i].Order;
			}
		}

		void SetAbility(AbilityType id, uint8 order) { Abilities[id].Order = order; }

		void AddExp(AbilityType id, int32 xp) {
			if (id != AbilityType::None) { Abilities[id].Exp += xp; }
		}

		std::unordered_map<AbilityType, Ability> Abilities;
	};

	struct Actability {
		Actability(uint32 id = 0) {
			Id = id;
		}

		uint32 Id = 0;
		int32 Point = 0;
		uint8 Step = 0;
	};

	struct CharacterActability {

		void AddPoint(uint32 id, int32 point) {
			if (!Actabilities.contains(id)) { return; }

			auto& act = Actabilities[id];
			act.Point += point;

			// TODO: Check expert limit and make sure it does not go over max
		}

		std::unordered_map<uint32, Actability> Actabilities;
	};

	struct CharacterSkills {
		
		void AddSkill(uint32 skillId) {
			//auto sTemplate = SkillManager::Instance().GetSkillTemplate(skillId);
			////if (sTemplate.AbilityId > 0 && sTemplate.AbilityId != )
			//auto points = ExperienceManager::Instance().GetSkillPointsForLevel(level);
			//points -= GetUsedSkillPoints();
			//if (sTemplate.SkillPoints > points) { return; }
			//
			//if (Skills.contains(skillId)) {
			//	SendPacket-> SCSkillLearnedPacket(Skills[skillId])...
			//}
			//else {
			//	AddSkill(sTemplate, 1, true);
			//}
		}

		void AddSkill(uint32 skillId, uint8 level, bool bSendPacket);

		void AddBuff(uint32 buffId) {

		}

		void Reset(AbilityType id) {

		}

		void Write(packet& p);

		// Probably let some handler calculate this
		int32 GetUsedSkillPoints() const {
			int32 points = 0;
			// ...
			return points;
		}

		// Let some handler calculate this
		bool IsVariantOfSkill(uint32 skillId) {
			return false;
		}

		std::vector<uint32> RemovedSkills; // ?? 
		std::unordered_map<uint32, Skill> Skills;
		std::unordered_map<uint32, PassiveBuff> PassiveBuffs;
	};

	/// <summary>
	/// Titles, unlocked titles...
	/// </summary>
	struct CharacterAppellations {

		uint32 ActiveAppellation = 0;
		/// Maybe store all appellations in database only.. cache only active?
		std::vector<uint32> Appellations;
	};

	struct CharacterVisualOptions {

		void Read(packet& p) {
			p >> Flag;
			if ((Flag & 1) == 1) {
				for (uint8 i = 0; i < 6; i++)
				{
					p >> Stp[i];
				}
			}
			if ((Flag & 2) == 2) {
				p >> Helmet;
			}
			if ((Flag & 4) == 4) {
				p >> BackHoldable;
			}
			if ((Flag & 8) == 8) {
				p >> Cosplay;
			}
			if ((Flag & 16) == 16) {
				p >> CosplayBackpack;
			}
		}

		void Write(packet& p) {
			p << Flag;
			if ((Flag & 1) == 1) {
				for (uint8 i = 0; i < 6; i++)
				{
					p << Stp[i];
				}
			}
			if ((Flag & 2) == 2) {
				p << Helmet;
			}
			if ((Flag & 4) == 4) {
				p << BackHoldable;
			}
			if ((Flag & 8) == 8) {
				p << Cosplay;
			}
			if ((Flag & 16) == 16) {
				p << CosplayBackpack;
			}
		}

		void WriteOptions(packet& p) {
			for (uint8 i = 0; i < 6; i++)
			{
				p << Stp[i];
			}
			p << Helmet << BackHoldable << Cosplay << CosplayBackpack;
		}

		uint8 Flag = 0;
		std::array<uint8, 6> Stp{};
		bool Helmet = false;
		bool BackHoldable = false;
		bool Cosplay = false;
		bool CosplayBackpack = false;
	};
}
