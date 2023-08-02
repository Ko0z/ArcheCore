#pragma once
#include "common_archecore.h"
#include "common_utils.h"
#include "common_entity.h"
#include "packet.h"

namespace e 
{
	
	struct ItemEquipable {
		uint8 Durability = 1;		// x
	};

	struct ItemGems {
		std::array<uint32, 7> GemIds{};		// x
	};

	//struct ItemRune {
	//	
	//};

	struct ItemChargeComp {
		time_t ChargeStartTime = 0;	// x  / either chargeStartTime or unpackTime depending on BindType
	};

	struct ItemEnchants {
		uint32 RuneId = 0;			// x
		uint16 TemperPhysical = 0;	// x
		uint16 TemperMagical = 0;	// x
		uint32 FusionId = 0;		// x
	};

	struct ItemLifeSpanComp {
		int32 LifeSpanMinutes = 0;	// x
		time_t CreatedTime = 0;		// x
		time_t ExpirationTime = 0;
		time_t UnsecureTime = 0;	// x
		time_t UnpackTime = 0;		// x
	};

	//[x] - Sent with every item packet
	struct ItemSlot {

		void Write(packet& p, entt::registry& world, entt::entity item) const;

		void WriteDetails(packet& p, entt::registry& world, entt::entity item) const;
		
		void WriteDetailsEquip(packet& p, entt::registry& world, entt::entity item, uint32 id) const;
		
		uint32 Id = 0;		// x database id
		uint64 Guid = 0;	// unique per instance id
		uint32 Count = 1;			// x
		SlotType SlotType = SlotType::None;		// x		// May describe what inv type it is inside (moot in our case)
		ItemFlag ItemFlag = ItemFlag::None;		// x		// Instance unique, Bind on Equip items etc..
		ItemDetailType DetailType = ItemDetailType::None;	// Can get info from prototype
		uint8 Grade = 0;
		ItemClassType Type = ItemClassType::ITEM;			// Could get from prototype
	};

	//struct ItemGradeComp {
	//	uint8 Grade;			// x
	//};

	// -----------[ Item Equipable Specific Components ]
	// Weapon, Armor and Accessory are non data additions, only calculations
	// 
								//[x] - Sent with every item packet if equipable
	

	//struct ItemTempers {
	//	
	//};

	// Transmog to itemId...
	//struct ItemFusion {
	//	
	//};

	// ----------[ Special Item Components ] ------>

	struct ItemBigFishComp {
		float Weight;
		float Length;
	};

	// Purely a tag for the item to know how to encode the data packet
	struct ItemBodyPart {};

	
}
