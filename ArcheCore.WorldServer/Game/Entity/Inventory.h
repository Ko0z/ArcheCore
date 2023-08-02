#pragma once

#include "common_archecore.h"
#include "common_entity.h"
#include "Entity/Items.h"
#include <vector>

struct packet;

namespace e 
{
	struct CharacterInventory {
		CharacterInventory(uint16 capacity) {
			Items.resize(capacity, entt::null);
		}

		void WriteFragmentedPacket(packet& p, uint8 startIndex, entt::registry& world) const;

		//void SetInitialItemExpirationTimers

		const size_t Capacity() const { return Items.size(); }

		std::vector<entt::entity> Items;
	};

	// All the items a player has in the bag
	struct InventoryBag : CharacterInventory 
	{
		SlotType SlotType() const { return SlotType::Inventory; }
	};

	struct InventoryWarehouse : CharacterInventory 
	{
		SlotType SlotType() const { return SlotType::Bank; }
	};

	struct InventoryEquipped {
		std::array<entt::entity, 28> Items;
	};

	struct InventoryMailAttachments {
		std::vector<entt::entity> Items;
	};

	struct InventorySystemContainer {
		std::vector<entt::entity> Items;
	};

}