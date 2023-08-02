#include "Character.h"
#include "packet.h"
#include "Managers/WorldManager.h"
#include "Managers/SkillManager.h"
#include "UnitCustomModelData.h"
#include "Entity/Transform.h"

namespace e {

	void Character::Write(packet& p, entt::registry& world, entt::entity entityCharacter) const
	{
		/// Get hold of all the components that we need in order to write this packet
		auto const& [unit, character, transform, hpmp, faction, exp, cInfo, family, laborComp, currency, combatSpec, customModel, equipItems]
			= world.get<Unit, Character, Transform, HealthMana, Faction, ExpComponent, CharacterInfo
			, Family, LaborComponent, CharCurrency, CombatSpec, UnitCustomModelData, InventoryEquipped>(entityCharacter);

		/// Start putting data in the packet
		p << character.Id << unit.Name.c_str() << (uint8)character.Race << (uint8)character.Gender << unit.Level << hpmp.Hp
			<< hpmp.Mp << transform.ZoneId << faction.FactionId << "" << (int32)0 << family.FamilyId;

		// ZoneId | x

		/// For every item that the character has equipped, we loop through and write the item specific data
		for (auto item : equipItems.Items)
		{
			if (item == entt::null) {
				p << (uint32)0;
				continue;
			}

			const e::ItemSlot& itemSlot = world.get<e::ItemSlot>(item);

			itemSlot.Write(p, world, item);
		}

		/// The character classes spec
		p << combatSpec.Spec1 << combatSpec.Spec2 << combatSpec.Spec3;

		/// Calculate the spawn position
		int64_t posX = ((int64_t)(transform.Position.x * 4096) << 32);
		int64_t posY = ((int64_t)(transform.Position.y * 4096) << 32);
		float_t posZ = (float_t)transform.Position.z;

		p << posX << posY << posZ;
		customModel.Write(p);

		/// All the trivial data
		p << laborComp.LaborPower << laborComp.LaborPowerModified << cInfo.DeadCount << cInfo.DeadTime << cInfo.RezWaitDuration
			<< cInfo.RezTime << cInfo.RezPenaltyDuration << cInfo.LeaveTime << currency.Money << (uint64)0 << currency.CrimePoints
			<< currency.CrimeRecord << (int16)0 << cInfo.DeleteRequestTime << cInfo.TransferRequestTime << cInfo.DeleteTime
			<< laborComp.ConsumedLaborPower << currency.LoyaltyTokens << currency.MoneyBank << (uint64)0 << cInfo.bUseAAPoint
			<< cInfo.PrevPoint << cInfo.Point << cInfo.Gift << cInfo.Updated << (uint8)0;
	}

    void ActionSlots::SetAction(uint8 slot, ActionSlotType type, uint32 actionId)
    {
		//std::cout << "Set Action Slot: " << (uint16)slot << " actionId: " << actionId << std::endl;
		Slots[slot].ActionType = type;
		Slots[slot].ActionId = actionId;
    }

	void CharacterSkills::AddSkill(uint32 skillId, uint8 level, bool bSendPacket)
	{
		const SkillTemplate* skillTemplate = SkillManager::Instance().GetSkillTemplate(skillId);
		if (skillTemplate) {
			Skill skill{ skillId, (uint8)1 };
			Skills.insert_or_assign(skillId, skill);
		}
		
		if (bSendPacket) {
			/// TODO: send packet about change...
		}
	}

	void CharacterSkills::Write(packet& p)
	{
		p << (uint8)Skills.size();

		for (auto& [key, skill] : Skills) {
			p << skill.Id << skill.Level;
		}

		p << (uint32)PassiveBuffs.size();

		for (auto& [key, buff] : PassiveBuffs) {
			p << buff.Id;
		}
	}

}
