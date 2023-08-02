#include "Inventory.h"
#include "packet.h"

namespace e {

	// SCCharacterInvenContentsPacket
	void CharacterInventory::WriteFragmentedPacket(packet& p, uint8 startIndex, entt::registry& world) const
	{
		// TODO: insert return statement here
		//std::cout << "Fragmented Packet: " << (uint16)startIndex << std::endl;
		for (auto i = startIndex; i < startIndex + 10; i++)
		{
			if (Items[i] == entt::null) {
				p << (uint32)0;
				//std::cout << (uint32)0 << " ";
				continue;
			}
			const e::ItemSlot& itemSlot = world.get<e::ItemSlot>(Items[i]);

			itemSlot.Write(p, world, Items[i]);
			//std::cout << itemSlot.Id << " ";
		}
		//std::cout << std::endl;
	}
	
	//void const CharacterInventory::WriteFragmentedPacket(packet& packet, entt::registry& world, entt::entity entity) const
	//{
	//	std::cout << "Write Base Inventoy" << std::endl;
	//
	//
	//}

	//void const InventoryBag::WriteFragmentedPacket(packet& packet, entt::registry& world, entt::entity entity) const
	//{
	//	
	//}
	//
	//void const InventoryWarehouse::WriteFragmentedPacket(packet& packet, entt::registry& world, entt::entity entity) const
	//{
	//	/// Check to see that we have items and that the number of all items is evenly divisble by 10
	//	if (Capacity == 0 || Capacity % 10 != 0) {
	//		std::cout << "[Inventory][Error] - Tried to write fragmented inventory packet with wrong inventory values" << std::endl;
	//		return;
	//	}
	//
	//}

	

}