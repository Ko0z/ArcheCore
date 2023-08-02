#include "Items.h"

namespace e {


void ItemSlot::Write(packet& p, entt::registry& world, entt::entity item) const
{
	
	if (Type == ItemClassType::BODYPART) {
		p << Id;
		return;
	}

	p << Id << Guid << Grade << (uint8)ItemFlag << Count << (uint8)DetailType;

	WriteDetails(p, world, item);

	const ItemLifeSpanComp& ls = world.get<ItemLifeSpanComp>(item);

	p << ls.CreatedTime << ls.LifeSpanMinutes << /* MadeUnitId */(uint32)0 << /* WorldId */(uint8)1
		<< ls.UnsecureTime << ls.UnpackTime;

}

void ItemSlot::WriteDetails(packet& p, entt::registry& world, entt::entity item) const
{
	switch (DetailType)
	{
	case ItemDetailType::Equipment:
	{
		// 55 bytes
		WriteDetailsEquip(p, world, item, Id);
		return;
	}
	case ItemDetailType::Slave:
		// 29 bytes
		return;
	case ItemDetailType::Mate:
		// 6 bytes
		return;
	case ItemDetailType::Ucc:
		// 9 bytes
		return;
	case ItemDetailType::Treasure:
		// 24 bytes
		return;
	case ItemDetailType::BigFish:
		// ?? 55 bytes ??
		return;
	case ItemDetailType::Decoration:
		// 16 bytes
		return;
	case ItemDetailType::MusicSheet:
		// 8 bytes
		return;
	case ItemDetailType::Glider:
		// 4 bytes
		return;
	}
}

void ItemSlot::WriteDetailsEquip(packet& p, entt::registry& world, entt::entity item, uint32 id) const
{
	auto const &[enchant, ls, itemEquip, gems]
		= world.get<ItemEnchants, ItemLifeSpanComp, ItemEquipable, ItemGems>(item);

	p << /*ImageItemTemplateId*/(uint32)0 << /*itemEquip.Durability*/(uint8)0x82 << /*??*/(uint16)0
		<< enchant.RuneId;

	//if (prot->BindType == ItemBindType::BindOnUnpack) {
	//	p << ls.UnpackTime;
	//}
	//else {
	//	p << archecore::utils::GetUnixTimeNowSigned();
	//}

	//p << archecore::utils::GetUnixTimeNowSigned();
	p << (int64)0;	// Time to 0
	p << /*?*/(uint32)0;

	for (size_t i = 0; i < 7; i++)
	{
		p << gems.GemIds[i];
	}

	p << enchant.TemperPhysical << enchant.TemperMagical;

}

}