#include "WorldSession.h"
#include "Opcodes.h"
#include "Managers/OpCodeManager.h"
#include "Managers/CharacterManager.h"
#include "Managers/WorldManager.h"
#include "Managers/ZoneManager.h"
#include "Managers/FactionManager.h"
#include "Entity/UnitCustomModelData.h"
#include "Entity/Movement.h"
#include "entt/entt.hpp"
#include "common_utils.h"
#include "Entity/Transform.h"

#include <guiddef.h>
#include <combaseapi.h>
#include <bitset>
//void WorldSession::ProcessEntityPacket(entt::registry& reg, packet& packet) {
//
//}

using namespace e;

using FunctionPointer = void (WorldSession::*)(packet& packet);

void WorldSession::ProcessPacket(entt::registry& reg, packet& packet)
{
	packet.resetReaderWriter();
	
	uint8_t unk; packet >> unk;
	uint8_t level; packet >> level;
	if (level == 1)
	{
		uint8_t crc;
		uint8_t counter;
		packet >> crc >> counter;
	}

	uint16_t opCode; packet >> opCode;
	packet.opcode = opCode;

	FunctionPointer handle = OpCodes::Instance().GetHandle(opCode, level);

	if (handle == nullptr) {
		//Handle_NULL(packet);
		return;
	}

	(this->*handle)(packet);
}

void WorldSession::BroadcastMessage(packet& p, std::vector<std::shared_ptr<WorldSession>>& connections, bool includeSelf)
{
	for (auto con : connections) {
		// TEMP
		if (con->inGame == true)
			con->Send(p);
	}
	//spdlog::info("Broadcasted packets to: {} connections of type 0x{:03x}", connections.size(), p.opcode);
	
	//if (includeSelf) {
	//	this->Send(p);
	//}
}

void WorldSession::Handle_NULL(packet& packet)
{
	// Unhandled OpCode so far...
}

// All OpCode Handlers
void WorldSession::X2EnterWorldResponsePacket(packet& packet)
{
	uint32_t token; packet >> token;

	uint16_t reason = 0x000;
	uint8_t gm = 0x00;
	uint16_t port = 60002;
	int64_t time_since_epoch = archecore::utils::GetUnixTimeNow();
	uint16_t pubKeySize = 0x000;
	const char* pubKey = "";

	packet.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x000);
	
	//			 2		 1		4		2			  8				   2		   1
	packet << reason << gm << token << port << time_since_epoch << pubKeySize << pubKey;

	this->Send(packet);

	ChangeStatePacket(0);
}

void WorldSession::CSLeaveWorldPacket(packet& packet)
{
	// TODO: Implement
	std::cout << "Player Left World, id: " << this->GetID() << std::endl;
}

void WorldSession::CSPremiumServieceMsgPacket(packet& packet)
{
	uint32_t stage; packet >> stage;

	char stageAscii = '0' + stage;
	//std::cout << "PremiumServieceMsg, stage: " << stageAscii << std::endl;
	
	SCAccountWarnedPacket(packet, 2, "Premium ...");
	// TODO: Implement
}

void WorldSession::ChangeStatePacket(int32_t state)
{
	packet p;
	p.prepareServerResponse(0xdd, 0x02, 0x000);

	p << state;
	//spdlog::info("--> ChangeStatePacket -->:{}", state);
	this->Send(p);
}

// Proxy 

void WorldSession::HandleFinishStatePacket(packet& packet)
{
	int32_t state; 
	packet >> state;

	//spdlog::info("FinishStatePacket state:{}", state);

	switch (state)
	{
	case 0:
		ChangeStatePacket(1);
		// If ActiveChar == null, we dont have an implementation for already created chars yet
		SetGameTypePacket("w_hanuimaru_1", 0, 1);
		SCInitialConfigPacket();

		// Can be ignored if not using shop I think
		SCTrionConfigPacket(true);
		SCAccountInfoPacket(1, 1, 0, 0);
		SCChatSpamDelayPacket();
		SCAccountAttributeConfigPacket();
		SCLevelRestrictionConfigPacket(10, 10, 10, 10, 10);
		break;
	case 1:
		ChangeStatePacket(2);
		break;
	case 2:
		ChangeStatePacket(3);
		break;
	case 3:
	case 4:
	case 5:
	case 6:
		ChangeStatePacket(state + 1);
		break;
	case 7:
		//std::cout << "HandleFinishStatePacket case 7 UNIMPLEMENTED SO FAR!!!!!" << std::endl;
		SCUpdatePremiumPointPacket(1, 1, 1);
		break;
	}
}

void WorldSession::HandlePingPacket(packet& packet)
{
	int64_t tm;
	int64_t when;
	uint32_t local;

	packet >> tm >> when >> local;

	// TODO: Save last ping, so that we can remove dead connections later
	uint32_t _world = GetTickCount();
	int64_t elapsed = 0;

	packet.prepareServerResponse(0xdd, 0x02, 0x013);
	packet << tm << when << elapsed << (int64_t)(_world * 1000) << local << _world;

	this->Send(packet);
}

void WorldSession::SetGameTypePacket(const char* levelname, uint64_t checksum, uint8_t immersive)
{
	packet p;

	p.prepareServerResponse(0xdd, 0x02, 0x00f);
	p << levelname << checksum << immersive;
	
	this->Send(p);
}

// G2C

void WorldSession::SCInitialConfigPacket() 
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x00f);
	
	const char* host = "aaemu.local";

	// TODO, much new here... check AAEmu -> SCInitialConfigPacket

	uint16_t bufferSize2 = 11;
	const uint8_t _fset[] = { 0x11, 0x37, 0x0F, 0x0F, 0x79, 0x69, 0xb3, 0x8d, 0x32, 0x0c, 0x1a };

	p << host << bufferSize2;

	for (uint8_t i = 0; i < 11; i++)
	{
		p << _fset[i];
	}

	int32_t configCount = 0;
	int32_t initLp = 50;
	bool canPlaceHouse = false;
	bool canPayTax = false;
	bool canUseAuction = true;
	bool canTrade = true;
	bool canSendMail = true;
	bool canUseBank = true;
	bool canUseCopper = true;
	uint8_t secondPriceType = 2;
	uint8_t secondPasswordMaxFailCount = 0;
	uint16_t idleKickTime = 0;

	p << configCount << initLp << canPlaceHouse << canPayTax << canUseAuction << canTrade << canSendMail
		<< canUseBank << canUseCopper << secondPriceType << secondPasswordMaxFailCount << idleKickTime;

	this->Send(p);
}

void WorldSession::SCTrionConfigPacket(bool activate) 
{
														// OLD TRION URL FOR SHOP
	const char* authUrl = "http://localhost/login/";	//"https://session.draft.integration.triongames.priv",
	const char* platformUrl = "http://localhost/platform/"; //"https://archeage.draft.integration.triongames.priv/commerce/purchase/credits/purchase-credits-flow.action",
	const char* commerceUrl = "http://localhost/shop/";		//"");
	
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x07);
	
	p << activate << ((uint16_t)(strlen(authUrl))) << authUrl << ((uint16_t)(strlen(platformUrl)))
		<< platformUrl << ((uint16_t)(strlen(commerceUrl))) << commerceUrl;

	this->Send(p);
}

void WorldSession::SCAccountInfoPacket(int32_t payMethod, int32_t payLocation, int64_t payStart, int64_t payEnd) 
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x1c2);

	int64_t realPayTime = 0;

	payStart = archecore::utils::GetUnixTimeNow();
	payEnd = payStart + 5184000;	// Pay end 60 days from now, hack,,,,

	p << payMethod << payLocation << payStart << payEnd << realPayTime;
	this->Send(p);
}

void WorldSession::SCChatSpamDelayPacket() 
{
	uint8_t version = 0;

	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x0d3);

	p << version;

	uint8_t chatTypeDelay = 0x0;
	for (uint8_t i = 0; i < 15; i++)
	{
		p << chatTypeDelay;
	}

	float_t chatGroupDelay = 0.0f;
	for (uint8_t i = 0; i < 15; i++)
	{
		p << chatGroupDelay;
	}

	p << "" << "";

	this->Send(p);
}

void WorldSession::SCAccountAttributeConfigPacket() 
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x215);
	
	bool first = false;
	bool second = true;
	p << first << second;

	this->Send(p);
}

void WorldSession::SCLevelRestrictionConfigPacket(uint8_t searchLevel, uint8_t bidLevel, uint8_t postLevel, uint8_t trade, uint8_t mail) 
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x217);

	// Should be a parameter later..
	const uint8_t limitLevel[] = { 0, 15, 15, 15, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 15 };

	// Payload
	p << searchLevel << bidLevel << postLevel << trade << mail;

	for (uint8_t i = 0; i < 15; i++)
	{
		p << limitLevel[i];
	}

	this->Send(p);
}

void WorldSession::SCUpdatePremiumPointPacket(int32_t point, uint8_t oldPg, uint8_t pG) {
	//0x201
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x201);

	p << point << oldPg << pG;

	this->Send(p);
}

void WorldSession::CSListCharacterPacket(packet& packet) 
{
	int32_t size;
	int16_t data;
	
	packet >> size >> data;

	SCGetSlotCountPacket(0);
	SCAccountInfoPacket(1, 1, 0, 0);
	SCRaceCongestionPacket();
	// If characters are 0 .. TODO: implement if chars exist on acc
	SCCharacterListPacket(true);
}

void WorldSession::CSRefreshInCharacterListPacket(packet& packet)
{
	SCRefreshInCharacterListPacket(packet);
}

void WorldSession::SCGetSlotCountPacket(uint8_t sc) 
{
	packet p;

	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x1e2);
	p << sc;

	this->Send(p);
}

void WorldSession::SCRaceCongestionPacket() 
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x3f);

	uint8_t zero = 0;

	for (uint8_t i = 0; i < 9; i++)
	{
		p << zero;
	}
	this->Send(p);
}

// TODO, make another param taking in an array of Character that has been grabbed from Database
void WorldSession::SCCharacterListPacket(bool last) 
{
	uint8_t charcterCount = 0;

	packet p;

	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x3e);
	p << last << charcterCount;

	this->Send(p);
}

void WorldSession::SCRefreshInCharacterListPacket(packet& p)
{
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x1cd);

	for (uint8_t i = 0; i < 9; i++)
	{
		p << (uint8_t)0;
	}
	this->Send(p);
}

void WorldSession::SCAccountWarnedPacket(packet& p, uint8_t source, const char* text)
{
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x1a1);
	p << source << text;

	this->Send(p);
}

void WorldSession::CSCreateCharacterPacket(packet& p)
{
	
	std::string name; p.read_bytes(name);
	uint8_t race, gender;

	p >> race >> gender;

	uint32_t body[7];
	for (size_t i = 0; i < 7; i++)
	{
		p >> body[i];
	}

	e::UnitCustomModelData customModel;
	customModel.Read(p);

	uint8_t ability1;
	uint8_t ability2;
	uint8_t ability3;
	uint8_t level;

	p >> ability1 >> ability2 >> ability3 >> level;

	//CharacterManager::Instance().Create(*this, name, race, gender, body, customModel, ability1);
	CharacterManager::Instance().Create(*this, name, race, gender, body, customModel, ability1);
	//m_Character = std::make_shared<Character>(character);
	
	// Create the character in a Character Manager class
	// 
	SCCreateCharacterResponsePacket(p);
}

void WorldSession::SCCreateCharacterResponsePacket(packet& p) {
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x038);

	entt::registry& world = WorldManager::Instance().GetDefaultWorld();

	const Character& character = world.get<Character>(entityCharacter);
	character.Write(p, world, entityCharacter);

	this->Send(p);
}

void WorldSession::CSRestrictCheckPacket(packet& p)
{
	uint32 characterId;
	uint8 code;

	p >> characterId >> code;

	uint8 result = 0;	// No restrictions

	SCResultRestrictCheckPacket(p, characterId, code, result);
}

void WorldSession::SCResultRestrictCheckPacket(packet& p, uint32 characterId, uint8 code, uint8 result)
{
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x1ce);

	p << characterId << code << result;

	this->Send(p);
}

void WorldSession::CSCharacterSelectPacket(packet& p)
{
	// LOTS TO DO
	
	uint32 charId;
	bool gm;

	p >> charId >> gm;
	spdlog::warn("Character ID: {} and GM:{}", charId, gm);

	bool characterFound = false;
	// An account can only have 4 characters...
	for (uint8 i = 0; i < 4; i++)
	{
		if (charId == characterIds[i]) {
			characterFound = true;
			break;
		}
	}
	if (characterFound == false) { 
		std::cout << "ERROR - CharacterSelectPacket: CharacterId not found on session." << std::endl;
		return; 
	}

	/// TODO: Actually load this character ID... Already in memory for now...
	entt::registry& world = WorldManager::Instance().GetDefaultWorld();
	
	packet p1;
	SCCharacterStatePacket(p1, world);	// Almost certainly Ok now
	this->Send(p1);

	packet p2;
	SCCharacterGamePointsPacket(p2, world);	// Same as AAEmu
	this->Send(p2);

	auto const& [bag, bank, actionSlots, actabilities, factionComponent] 
		= world.get<InventoryBag, InventoryWarehouse, ActionSlots, CharacterActability, Faction>(entityCharacter);

	packet p3;
	SCCharacterInvenInitPacket(p3, bag, bank);	// Same as AAEmu
	this->Send(p3);

	/// Send Bag Inventory Fragment packets...
	//std::cout << "WRITING BAG" << std::endl;
	// Check to see that we have items and that the number of all items is evenly divisble by 10
	auto bagCapacity = bag.Capacity();
	if (bagCapacity == 0 || bagCapacity % 10 != 0) {
		std::cout << "[Inventory][Error] - Tried to write fragmented inventory packet with wrong inventory values" << std::endl;
		return;
	}

	auto numBagPackets = bagCapacity / 10;
	for (uint8 i = 0; i < numBagPackets; i++)
	{
		packet x;
		SCCharacterInvenContentsPacket(x, i, world, bag, bag.SlotType());
		this->Send(x);
	}

	/// Send Warehouse Inventory Fragment packets...
	//std::cout << "WRITING BANK" << std::endl;
	auto bankCapacity = bank.Capacity();
	// Check to see that we have items and that the number of all items is evenly divisble by 10
	if (bankCapacity == 0 || bankCapacity % 10 != 0) {
		std::cout << "[Inventory][Error] - Tried to write fragmented inventory packet with wrong inventory values" << std::endl;
		return;
	}

	auto numBankPackets = bankCapacity / 10;
	for (uint8 i = 0; i < numBankPackets; i++)
	{
		packet x;
		SCCharacterInvenContentsPacket(x, i, world, bank, bank.SlotType());
		this->Send(x);
	}

	/// Send ActionSlotsPacket...
	SCActionSlotsPacket_Send(actionSlots);	// Checked OK

	///Send Quests.. 0 for empty now..
	SCQuestsPacket_Send();					// Checked OK

	///Send Completed Quests..
	SCCompletedQuestsPacket_Send();			// Checked OK

	///Send Actabilities..
	SCActabilityPacket_Send(actabilities, true);	// Checked OK

	///Send UnreadMailCount..
	SCCountUnreadMailPacket_Send();		// Checked OK

	///Send Appellations..	x dont need
	
	///Send Portals..		DO NEED
	// Hack Nuian starting zone for now...
	uint32 num = 1;
	uint32 zoneId = 243;	// Nuian starting zone
	const char* portalName = "Nuian Starting Area";

	packet pPortal;
	pPortal.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16_t)SC_OpCode::SCCharacterReturnDistrictsPacket);
	
	pPortal << num << zoneId << portalName << (uint32)179;
	
	const uint8_t unknown[] = { 0x58, 0x41, 0x9b, 0x44, 0xf4, 0x61, 0x01, 0x45, 0xcf, 0xf7, 0xfc, 0x42 };
	
	for (uint8 i = 0; i < 12; i++)
	{
		pPortal << unknown[i];
	}
	const uint8_t unknown2[] = { 0x00, 0x00, 0x68, 0x42, 0xf3, 0x00, 0x00, 0x00};
	
	for (uint8 i = 0; i < 8; i++)
	{
		pPortal << unknown2[i];
	}
	Send(pPortal);

	///Send Friends..		x dont need
	///Send Blocked..		x dont need
	///Send Houses..		x dont need
	
	///Send Conflicts..		
	for (const auto& [id, conflict] : ZoneManager::Instance().GetZoneConflicts()) {
		SCConflictZoneStatePacket_Send(id, conflict.CurrentZoneState, conflict.NextStateTime);
	}

	///Send Faction List..
	SCFactionListPacket();		// Seems ok

	///Send Faction Relations..
	SCFactionRelationListPacket(FactionManager::Instance().GetRelations());		// Exactly same as AAEmu

	///Send Expeditions (Party ? Group)..
	SCExpeditionRolePolicyListPacket_Send();

	///Send Options UI stuff 1, 2, 5..
	//spdlog::info("Sending Options..");
	//
	std::string string1 = "version 1";
	std::string string2 = "version 2";
	std::string string5 = "version 1";
	
	SCResponseUIDataPacket_Send(charId, 1, string1);
	SCResponseUIDataPacket_Send(charId, 2, string2);
	SCResponseUIDataPacket_Send(charId, 5, string5);
	
	/// 
	/// Add Logged in buff	// x
	/// Hack for now, set Logged on buff
	
	//packet buffPacket;
	//buffPacket.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16_t)SC_OpCode::SCBuffCreatedPacket);
	//
	//buffPacket << (uint8)0;	// Buff Type
	//
	//// the ObjId ( if the buff type is 0, meaning Unit, then it is the unit ObjId
	//buffPacket << (uint8)0x3d << (uint8)0x05 << (uint8)0x00;
	//buffPacket << charId;	// if unit is Player, then characterId, otherwise 0
	//buffPacket << (uint8)0x3d << (uint8)0x05 << (uint8)0x00;	// target ObjId
	//			//	index			buffID		casterLvl	buffLvl		if skillId		WritePisc..
	//buffPacket << (uint32)1 << (uint32)2423 << (uint8)1 << (uint32)1 << (uint16)0 << (uint16)4 << (uint32)2000;
	//Send(buffPacket);

	// Join chat message...

	//SCJoinedChatChannelPacket(factionComponent);
	/// Set Character Breath capacity
	/// 
	/// OnZoneChange...
}

void WorldSession::SCCharacterStatePacket(packet& p, entt::registry& world)
{
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16_t)SC_OpCode::SCCharacterStatePacket);
	
	/// Get hold of all the components that we need in order to write this packet
	auto const& [character, exp, cInfo, currency, unit]
		= world.get<Character, ExpComponent, CharacterInfo, CharCurrency, Unit>(entityCharacter);

	//GUID guid;
	//CoCreateGuid(&guid);

	uint32 instanceId = 0;
	uint64 guid = 240000;	//
	uint16 guidSize = 16; // how many bytes the guid should be

	p << instanceId << guidSize << guid << guid << /*rwd?*/(int32)0;

	character.Write(p, world, entityCharacter);

	const uint8_t angles[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDB, 0xFB, 0x17, 0xC0 };

	/// We should not send the array size before this.
	for (uint8 i = 0; i < 12; i++)
	{
		p << angles[i];
	}
										/*penaltiedExp*//*cInfo.ReturnDistrictId*//*returnDistrict*//*cInfo.ResurrectionDistrictId*/
	p << exp.Experience << exp.RecoverableExp << (uint32)0 << (uint32)0 <<(uint32)0 << (uint32)0;

	for (uint8 i = 0; i < 11; i++)
	{		/*AbilityExp*/
		p << (uint32)0;
	}

	p << /*unreadMailCount.Received*/(int32)0 << /*unreadMailCount.MiaReceived*/ (int32)0
		<< /*unreadMailCount.CommercialReceived*/ (int32)0 << /*numInventorySlots*/(uint8)50
		<< /*numBankSlots*/(uint16)50 << currency.Money << currency.MoneyBank << /*money..?*/(int64)0 << /*money..?*/(int64)0
		<< cInfo.bUseAAPoint << /*juryPoint*/(int32)0 << /*jailSeconds*/(int32)0 << /*bountyMoney*/(int64)0
		<< /*bountyTime*/(int64)0 << /*reportedNo*/(int32)0 << /*suspectedNo*/(int32)0 << /*totalPlayTime*/(int32)0
		<< cInfo.Created << /*expandedExpert*/(uint8)0;

}

void WorldSession::SCCharacterGamePointsPacket(packet& p, entt::registry& world)
{
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16_t)SC_OpCode::SCCharacterGamePointsPacket);

	const CharCurrency& charPoints = world.get<CharCurrency>(entityCharacter);

	p << charPoints.HonorPoints << charPoints.VocationPoints;

	//std::cout << "honor: " << charPoints.HonorPoints << " vocation: " << charPoints.VocationPoints << " ";

	for (auto i = 0; i < 8; i++)
	{
		p << (int32)0;	// Different kind of points I guess, CrimePoints etc...
	}
}

void WorldSession::SCCharacterInvenInitPacket(packet& p, const e::InventoryBag& bag, const e::InventoryWarehouse& bank) const
{
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16_t)SC_OpCode::SCCharacterInvenInitPacket);

	p << (uint32)bag.Capacity() << (uint32)bank.Capacity();
	//std::cout << "bagCapacity: " << bag.Capacity() << "bankCapacity: " << bank.Capacity() << std::endl;
}

void WorldSession::SCCharacterInvenContentsPacket(packet& p, uint8 startIndex, entt::registry& world, const CharacterInventory& inv, SlotType invType) const
{
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16_t)SC_OpCode::SCCharacterInvenContentsPacket);

	p << (uint8)invType << /*numChunks*/(uint8)1 << startIndex;

	inv.WriteFragmentedPacket(p, startIndex * 10, world);

	// TODO, send itemExpireTimes ...
}

void WorldSession::SCActionSlotsPacket_Send(e::ActionSlots& actionSlotsComponent)
{
	/// Checked, is OK, doesnt include 2 potions that AAEmu put in starter inventory, otherwise same
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16_t)SC_OpCode::SCActionSlotsPacket);
	
	int16 i = 0;
	for (auto& slot : actionSlotsComponent.Slots) {
		ActionSlotType slotType = slot.ActionType;

		p << (uint8)slotType;

		switch (slotType)
		{
			case e::ActionSlotType::None:
				break;
			case e::ActionSlotType::ItemType:
			case e::ActionSlotType::Spell:
			case e::ActionSlotType::RidePetSpell:
				p << (uint32)slot.ActionId;
				break;
			case e::ActionSlotType::ItemId:
				p << slot.ActionId;
				break;
			
		}
		
	}
	
	Send(p);
}

void WorldSession::SCQuestsPacket_Send()
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCQuestsPacket);

	// TODO, send actual quests
	p << (int32)0;	// for empty... (quest vector length)
	Send(p);
}

void WorldSession::SCCompletedQuestsPacket_Send()
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCCompletedQuestsPacket);

	//p << (int32)0;	// for empty... (completedQuests vector length)
	
	/// Hack for now... You start with one quest completed...
	uint32 one = 1;
	uint32 questId = 40; // this is a uint16 normally, and then the packet has an 8 byte bitarray for mask

	p << one << questId << (uint8)0 << (uint8)1 << (uint32)0;

	Send(p);
}

void WorldSession::SCActabilityPacket_Send(CharacterActability& actabilities, bool last)
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCActabilityPacket);

	/// TODO: fix so that we only send max 100 each packet, then set last = true, on the last packet
	p << last << (uint8)actabilities.Actabilities.size();

	for (const auto& [key, value] : actabilities.Actabilities) {
		p << value.Id << value.Point << value.Step;
	}
	Send(p);
}

void WorldSession::SCCountUnreadMailPacket_Send()
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCCountUnreadMailPacket);

	int32 sent = 0;
	int32 received = 0;
	int32 mIAReceived = 0;
	int32 commercialReceived = 0;

	p << sent << received << mIAReceived << commercialReceived;

	Send(p);
}

void WorldSession::SCAppellationsPacket_Send(CharacterAppellations& appellations)
{
	for (size_t i = 0; i < appellations.Appellations.size(); i += 512)
	{
		// TODO
	}
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCAppellationsPacket);



	Send(p);
}

void WorldSession::SCConflictZoneStatePacket_Send(uint16 id, ZoneConflictType conflictType, time_t endTime)
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCConflictZoneStatePacket);

	time_t peaceStart = archecore::utils::GetUnixTimeNow();
	time_t peaceEnd = peaceStart + 86400;	// Peace for 24h for now...

	p << id << (uint8)conflictType << peaceEnd;

	Send(p);
}

void WorldSession::SCExpeditionRolePolicyListPacket_Send()
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCExpeditionRolePolicyListPacket);
					// Hack value...
	p << (uint8)0 << (uint16)29;	// Just send 0 party for now
	Send(p);
}

void WorldSession::SCResponseUIDataPacket_Send(uint32 characterId, uint16 key, std::string& theString)
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCResponseUIDataPacket);
	
	p << characterId << key;

	if (key == 1) {							/// version 1
		const uint8_t optionString1[] = { 0x0b, 0x00, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x31, 0x0d, 0x0a, 0x0c, 0x00, 0x00, 0x00 };
		for (uint8 i = 0; i < 17; i++)
		{
			p << optionString1[i];
		}
	}
	else if (key == 2) {					/// version 2
		const uint8_t optionString2[] = { 0x0b, 0x00, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x32, 0x0d, 0x0a, 0x0c, 0x00, 0x00, 0x00 };
		for (uint8 i = 0; i < 17; i++)
		{
			p << optionString2[i];
		}
	}
	else if (key == 5) {
		p << (uint8)0xe3 << (uint8)0x00 << (uint8)0x22 << (uint8)0xef << (uint8)0xbf << (uint8)0xbd << (uint8)0x22 << (uint8)0x0d << (uint8)0x0a << (uint8)0x72 << (uint8)0x6f << (uint8)0x61 << (uint8)0x64 << (uint8)0x6d << (uint8)0x61				// ..."..."..roadma
		<< (uint8)0x70 << (uint8)0x5f << (uint8)0x6f << (uint8)0x70 << (uint8)0x74 << (uint8)0x69 << (uint8)0x6f << (uint8)0x6e << (uint8)0x20 << (uint8)0x28 << (uint8)0x20 << (uint8)0x73 << (uint8)0x69 << (uint8)0x7a << (uint8)0x65 << (uint8)0x20  // p_option(size
		<< (uint8)0x33 << (uint8)0x2c << (uint8)0x20 << (uint8)0x76 << (uint8)0x69 << (uint8)0x73 << (uint8)0x69 << (uint8)0x62 << (uint8)0x6c << (uint8)0x65 << (uint8)0x20 << (uint8)0x74 << (uint8)0x72 << (uint8)0x75 << (uint8)0x65 << (uint8)0x2c  // 3, visible true,
		<< (uint8)0x20 << (uint8)0x6e << (uint8)0x70 << (uint8)0x63 << (uint8)0x53 << (uint8)0x68 << (uint8)0x6f << (uint8)0x77 << (uint8)0x20 << (uint8)0x66 << (uint8)0x61 << (uint8)0x6c << (uint8)0x73 << (uint8)0x65 << (uint8)0x20 << (uint8)0x29  //  npcShow false)
		<< (uint8)0x0d << (uint8)0x0a << (uint8)0x71 << (uint8)0x75 << (uint8)0x65 << (uint8)0x73 << (uint8)0x74 << (uint8)0x5f << (uint8)0x6e << (uint8)0x6f << (uint8)0x74 << (uint8)0x69 << (uint8)0x66 << (uint8)0x69 << (uint8)0x65 << (uint8)0x72  // ..quest_notifier
		<< (uint8)0x5f << (uint8)0x6c << (uint8)0x69 << (uint8)0x73 << (uint8)0x74 << (uint8)0x0d << (uint8)0x0a << (uint8)0x20 << (uint8)0x20 << (uint8)0x20 << (uint8)0x20 << (uint8)0x6c << (uint8)0x69 << (uint8)0x73 << (uint8)0x74 << (uint8)0x4f  // _list..listO
		<< (uint8)0x70 << (uint8)0x65 << (uint8)0x6e << (uint8)0x20 << (uint8)0x74 << (uint8)0x72 << (uint8)0x75 << (uint8)0x65 << (uint8)0x0d << (uint8)0x0a << (uint8)0x20 << (uint8)0x20 << (uint8)0x20 << (uint8)0x20 << (uint8)0x6f << (uint8)0x70  // pen true..op
		<< (uint8)0x65 << (uint8)0x6e << (uint8)0x48 << (uint8)0x65 << (uint8)0x69 << (uint8)0x67 << (uint8)0x68 << (uint8)0x74 << (uint8)0x20 << (uint8)0x33 << (uint8)0x30 << (uint8)0x30 << (uint8)0x0d << (uint8)0x0a << (uint8)0x20 << (uint8)0x20  // enHeight 300..
		<< (uint8)0x20 << (uint8)0x20 << (uint8)0x71 << (uint8)0x75 << (uint8)0x65 << (uint8)0x73 << (uint8)0x74 << (uint8)0x4c << (uint8)0x69 << (uint8)0x73 << (uint8)0x74 << (uint8)0x0d << (uint8)0x0a << (uint8)0x20 << (uint8)0x20 << (uint8)0x20  //   questList..
		<< (uint8)0x20 << (uint8)0x64 << (uint8)0x65 << (uint8)0x63 << (uint8)0x61 << (uint8)0x6c << (uint8)0x53 << (uint8)0x74 << (uint8)0x61 << (uint8)0x74 << (uint8)0x65 << (uint8)0x73 << (uint8)0x20 << (uint8)0x28 << (uint8)0x20 << (uint8)0x30  //  decalStates(0
		<< (uint8)0x2c << (uint8)0x20 << (uint8)0x30 << (uint8)0x2c << (uint8)0x20 << (uint8)0x30 << (uint8)0x2c << (uint8)0x20 << (uint8)0x30 << (uint8)0x2c << (uint8)0x20 << (uint8)0x30 << (uint8)0x2c << (uint8)0x20 << (uint8)0x30 << (uint8)0x2c	//, 0, 0, 0, 0, 0,
		<< (uint8)0x20 << (uint8)0x30 << (uint8)0x2c << (uint8)0x20 << (uint8)0x30 << (uint8)0x2c << (uint8)0x20 << (uint8)0x30 << (uint8)0x2c << (uint8)0x20 << (uint8)0x30 << (uint8)0x20 << (uint8)0x29 << (uint8)0x0d << (uint8)0x0a << (uint8)0x71  //  0, 0, 0, 0)..q
		<< (uint8)0x75 << (uint8)0x65 << (uint8)0x73 << (uint8)0x74 << (uint8)0x5f << (uint8)0x63 << (uint8)0x6f << (uint8)0x6e << (uint8)0x74 << (uint8)0x65 << (uint8)0x78 << (uint8)0x74 << (uint8)0x5f << (uint8)0x73 << (uint8)0x74 << (uint8)0x61  // uest_context_sta
		<< (uint8)0x74 << (uint8)0x65 << (uint8)0x5f << (uint8)0x76 << (uint8)0x61 << (uint8)0x6c << (uint8)0x75 << (uint8)0x65 << (uint8)0x73 << (uint8)0x0d << (uint8)0x0a << (uint8)0x76 << (uint8)0x65 << (uint8)0x72 << (uint8)0x73 << (uint8)0x69  // te_values..versi
		<< (uint8)0x6f << (uint8)0x6e << (uint8)0x20 << (uint8)0x31 << (uint8)0x0d << (uint8)0x0a << (uint8)0xe2 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00;                   // on 1......
	}
		
	Send(p);
}

void WorldSession::SCJoinedChatChannelPacket(Faction& factionComponent)
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCJoinedChatChannelPacket);
	p << (uint16)1 << (uint16)5 << factionComponent.FactionId << (uint16)0;

	Send(p);
}

// TODO: Add worldId as parameter so we know where to look for entity data... etc...
void WorldSession::SCUnitStatePacket(e::BaseUnitType unitType, entt::entity entity, bool toSelf)
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCUnitStatePacket);

	entt::registry& world = WorldManager::Instance().GetDefaultWorld();

	auto const& [objId, unit, transform, equipItems, customModel, hpMp, skills, character, faction, spec, specAbilites, transmogs]
		= world.get<ObjectId, Unit, Transform, InventoryEquipped, UnitCustomModelData, 
					HealthMana, CharacterSkills, Character, Faction, CombatSpec, 
					CharacterAbilites, CharacterVisualOptions>(entity);

	//uint8 buffer[3]{};
	//archecore::utils::getLSB(unit.Id, 3, buffer);
	
	//p << unit.guid[0] << unit.guid[1] << unit.guid[2] << unit.Name.c_str() << (uint8)unitType;
	
	//uint32 uId = static_cast<uint32>(entityCharacter);
	//
	//if (world.valid((entt::entity)uId)) {
	//	spdlog::info("is Valid!");
	//}

	p.WriteBc(objId.Id);
	p << unit.Name.c_str() << (uint8)unitType;
	
	if (unitType == BaseUnitType::CHARACTER) {
		p << unit.Id << (uint64)0; // ?
	}
	else 
	{
		spdlog::error("SCUnitStatePacket is not handling this type yet.. UnitType: {}", (uint16)unitType);
		return;
	}

	// If unit has an owner, write ID of owner
	p << "";

	p.WritePosition(transform.Position);

	//std::array<uint8, 9> pos = archecore::utils::ConvertPosition(spawn.X, spawn.Y, spawn.Z);
	
	//spdlog::info("Initial Pos, X1:{}, X2:{}, X3:{} - Y1:{}, Y2:{}, Y3:{} - Z1:{}, Z2:{}, Z3:{}",
	//	pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7], pos[8]);
	
	//for (uint8 i = 0; i < 9; i++)
	//{
	//	p << pos[i];
	//}

	p << (float)1 << unit.Level << unit.ModelId;

	//int32 validFlags = 0;
	//int32 index = 0;
	//for (auto item : equipItems.Items) {
	//	if (item != entt::null) {
	//		validFlags |= 1 << index;
	//	}
	//	index++;
	//}
	//if (validFlags <= 0 && unitType == BaseUnitType::NPC) {
	//	// Change CustomModelData to type skin... // additional check that the NPC has no body and face
	//}

	/// TODO: if cosplay (transmog) item, do something else whatevs
	int32 index = 0;
	for (auto item : equipItems.Items) {
		if (item == entt::null) {
			p << (uint32)0;
			continue;
		}

		const ItemSlot& itemSlot = world.get<ItemSlot>(item);

		itemSlot.Write(p, world, item);
	}
	customModel.Write(p);
	p << ((uint8)0) << ((uint8)0) << ((uint8)0);	// Instead of doing that convert stuff from 4 bytes to 3.... 
	p << (hpMp.Hp * 100) << (hpMp.Mp * 100);

	p << (uint8)AttachPointKind::System;	// AttachPoint 1
	p << (int8)-1;

	// TODO, write proper ModelPosture type...
									// isLooted
	p << (uint8)unit.ModelPosture << false;
		// activeWeaponBySlot?
	p << (uint8)0;

	if (unitType == BaseUnitType::CHARACTER) {
		skills.Write(p);
	}
	else {
		p << (uint8)0 << (int32)0; // learnedSkillCount, learnedBuffCount
	}
	//glm::vec3 eulerRotation = transform.GetEulerRotationDegrees();
	//archecore::utils::CompressedRotation cRotation(eulerRotation.x, eulerRotation.y, eulerRotation.z);
	//p << cRotation.Roll << cRotation.Pitch << cRotation.Yaw;

	glm::i8vec3 sByteRotation = transform.GetSByteRotation();
	p.WriteRotation(sByteRotation);

	p << character.GetRaceGender();
	// Hack for now..	// numActiveAppellations with pisc writing...
	p << (uint8)0 << (uint32)0;
	// Same but with factions...							// expeditionId
	p << (uint8)0 << (uint8)faction.FactionId << (uint8)0 << (uint8)0 << (uint8)0;

	//p << (uint32)0 << (uint16)0 << (uint8)0;
	//std::bitset<16> flags;
	// if invisible, put flag 5 to "true", if Idle put bit 13 to "true"
	//	 pisc?   honorGainedCombat HostileFactionKills
	//p << (uint8)0 << (uint8)0 << (uint8)0;
	specAbilites.SetAbility(AbilityType::General, 0);

	specAbilites.Write(p);

	p << spec.GetNumActive();
	spec.Write(p);

	p << ((uint8)0) << ((uint8)0) << ((uint8)0);	// Instead of doing that convert stuff from 4 bytes to 3.... 

	transmogs.WriteOptions(p);	// ...'cosplay'

	p << (int32)1;	// premium... 
	
	p << (int32)0 << (int32)0 << (int32)0 << (int32)0 << (int32)0 << (int32)0;	// pStat??

	
	p << (uint8)0;	// Good buffs count
	p << (uint8)0;	// Bad buffs count
	p << (uint8)0;	// Hidden buffs count

	//
	//packet buffPacket;
	//buffPacket.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16_t)SC_OpCode::SCBuffCreatedPacket);
	//
	//buffPacket << (uint8)0;	// Buff Type
	//
	//// 3 byte wierd shit // that is the guid...
	//buffPacket << (uint8)0x3d << (uint8)0x05 << (uint8)0x00;
	//buffPacket << unit.Id;
	//buffPacket << (uint8)0x3d << (uint8)0x05 << (uint8)0x00;
	//// buffID
	//buffPacket << (uint32)1 << (uint32)2423 << (uint8)1 << (uint32)1 << (uint16)0 << (uint16)4 << (uint32)2000;
	//Send(buffPacket);

	//p << (uint8)0;	// Good buffs count
	//p << (uint8)0;	// Bad buffs count
	//p << (uint8)0;	// Hidden buffs count

	// Hack for buffs for now...																					//----------------objId----------------//
	//p << (uint8)0x00 << (uint8)0x00 << (uint8)0x77 << (uint8)0x09 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x3d << (uint8)0x05 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00;
	//p << (uint8)0x01 << (uint8)0x01 << (uint8)0x00 << (uint8)0x20 << (uint8)0x4e << (uint8)0x00 << (uint8)0x00 << (uint8)0x43 << (uint8)0x0c << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00;
	//p << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x01 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x04;
	//p << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x0b << (uint8)0x12 << (uint8)0x7a << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x01;
	//p << (uint8)0x01 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x01 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00;
	//p << (uint8)0x00 << (uint8)0x00 << (uint8)0x01 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x05 << (uint8)0x00;
	//p << (uint8)0x00 << (uint8)0x00 << (uint8)0x0c << (uint8)0x12 << (uint8)0x7a << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x01 << (uint8)0x01;
	//p << (uint8)0x00 << (uint8)0x00 << (uint8)0x84 << (uint8)0x0c << (uint8)0x24 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00;
	//p << (uint8)0x00 << (uint8)0x01 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00 << (uint8)0x00;
	if (toSelf == true) {
		Send(p);
	}
	else {
		std::vector<std::shared_ptr<WorldSession>> connections = WorldManager::Instance().GetNearbyConnections(objId.Id, id);
		BroadcastMessage(p, connections, false);
	}
	
	//SCDetailedTimeOfDayPacket();
}

void WorldSession::SCDetailedTimeOfDayPacket()
{
	float time = 43200.0 / 3600.0;
	float speed = 0.0016666;
	float start = 0.0;
	float end = 24.0;

	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCDetailedTimeOfDayPacket);

	p << time << speed << start << end;
	Send(p);
}

void WorldSession::SCOneUnitMovementPacket(uint32 objId, MovementPacketData& moveData, bool includeSelf)
{
	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCOneUnitMovementPacket);

	p.WriteBc(objId);
	p << (uint8)moveData.Type;

	moveData.Write(p);

	std::vector<std::shared_ptr<WorldSession>> connections = WorldManager::Instance().GetNearbyConnections(objId);
	BroadcastMessage(p, connections, includeSelf);
}

void WorldSession::CSSetLpManageCharacterPacket(packet& p)
{
	uint32 characterId;

	// TODO, check if we get more data in the packet maybe?
	p >> characterId;

	SCLpManagedPacket(p, characterId);
}

void WorldSession::CSSaveUIDataPacket(packet& p)
{
	uint16 uiDataType;
	uint32 id;
	std::string dataString; 

	p >> uiDataType >> id;
	p.read_bytes(dataString);

	//spdlog::info("UI Data: type: {} , id: {} , string: ", uiDataType, id);
	std::cout << dataString << std::endl;
}

void WorldSession::CSSpawnCharacterPacket(packet& p)
{
	entt::registry& world = WorldManager::Instance().GetDefaultWorld();
	CharacterVisualOptions& visualComp = world.emplace<CharacterVisualOptions>(entityCharacter);
	visualComp.Read(p);

	SCUnitStatePacket(BaseUnitType::CHARACTER, entityCharacter);
}

void WorldSession::CSNotifyInGamePacket(packet& p)
{
	entt::registry& world = WorldManager::Instance().GetDefaultWorld();
	//PlayerSession& playerSessionComp = world.get<PlayerSession>(entitySession);
	//
	//playerSessionComp.InWorld = true;
	
	// Update this character about everyone else
	auto query = world.view<Transform, WorldComponent>();
	for (auto& entity : query) {
		if (entity != entityCharacter) {
			SCUnitStatePacket(BaseUnitType::CHARACTER, entity, true);
		}
	}

	WorldManager::Instance().SpawnCharacter(entityCharacter, WorldManager::WORLD_DEFAULT_ID);
	inGame = true;

	// Update everyone else about this new character
	SCUnitStatePacket(BaseUnitType::CHARACTER, entityCharacter, false);

	//spdlog::info("In game?");
	// Spawn
	spdlog::info("CSNotifyInGamePacket: {}", id);
}

void WorldSession::CSMoveUnitPacket(packet& p)
{
	uint32 objId;
	EMoveType moveType;
	uint8 i;

	p.ReadBc(objId);
	p >> i;
	
	//spdlog::info("CSMoveUnitPacket for objId: {}", objId);

	moveType = EMoveType(i);
	MovementPacketData moveData(moveType);
	moveData.Read(p);

	entt::registry& world = WorldManager::Instance().GetDefaultWorld();
	entt::entity movedEntity = WorldManager::Instance().GetEntityByObjId(objId);

	switch (moveData.Type)
	{
		case e::EMoveType::DEFAULT:
			break;
		case e::EMoveType::UNIT:
			moveData.HandleUnitMovement(world, movedEntity);
			break;
		case e::EMoveType::VEHICLE:
			break;
		case e::EMoveType::SHIP:
			break;
		case e::EMoveType::SHIP_REQUEST:
			break;
		case e::EMoveType::TRANSFER:
			break;
	}
	
	// Broadcast to everyone near entity object
	SCOneUnitMovementPacket(objId, moveData, true);
}

void WorldSession::CSNotifySubZonePacket(packet& p)
{
	uint32 subZoneId;
	p >> subZoneId;

	/// TODO: Update the recall location for this player
	spdlog::info("Connection: {} entered region: {}", id, subZoneId);
}

void WorldSession::CSNotifyInGameCompletedPacket(packet& p)
{
	/// Client has now entered the world..
	/// Set if its snow and family notification etc...
	//WorldManager::Instance().OnPlayerJoin();

	spdlog::info("CSNotifyInGameCompletedPacket: {}", id);
}

void WorldSession::SCLpManagedPacket(packet& p, uint32 characterId)
{
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCLpManagedPacket);

	p << characterId;

	this->Send(p);
}

void WorldSession::SCCharacterCreationFailedPacket(uint8_t reason, packet& p)
{
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, 0x03d);
	p << reason;

	this->Send(p);
}

void WorldSession::SCFactionListPacket()
{
	std::map<uint32_t, SystemFaction>& factions = FactionManager::Instance().GetFactions();
	
	packet p1;
	p1.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCFactionListPacket);
	p1 << (uint8)20;

	packet p2;
	p2.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCFactionListPacket);
	p2 << (uint8)20;

	packet p3;
	p3.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCFactionListPacket);
	p3 << (uint8)20;

	packet p4;
	p4.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCFactionListPacket);
	p4 << (uint8)20;

	packet p5;
	p5.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCFactionListPacket);
	p5 << (uint8)2;

	uint32 counter = 0;

	for (auto& [key, v] : factions) {
		if (counter < 20) {
			v.Write(p1);
			counter++;
		}
		else if (counter < 40) {
			v.Write(p2);
			counter++;
		}
		else if (counter < 60) {
			v.Write(p3);
			counter++;
		}
		else if (counter < 80) {
			v.Write(p4);
			counter++;
		}
		else {
			v.Write(p5);
			counter++;
		}
	}

	Send(p1);
	Send(p2);
	Send(p3);
	Send(p4);
	Send(p5);
}

void WorldSession::SCFactionRelationListPacket(std::vector<FactionRelation>& relations)
{
	// 1x FactionRelation size footprint is 26 bytes ( 17 byte from struct + 9 byte unknown )
	// there are 1174 entries of FactionRelation in database, 1174 * 26 = 30524 byte.
	// Send max 157 per packet in a buffer size of 4096 bytes. 157 * 26 = 4082
	/// An old hero wrote this code...
	
	//spdlog::info("NumRelations: {}", relations.size());

	for (uint16 i = 0; i < 5; i++)
	{
		packet p;
		p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCFactionRelationListPacket);
		p << false;	// uiRequest
		p << (uint8_t)200;	// numElements

		//time_t timeFuture = archecore::utils::GetUnixTimeNowSigned() + 86400;	//12 h from now

		for (uint16 j = i*200; j < 200 + (200 * i); j++)
		{
			p << relations[j].id << relations[j].id2 << (uint8_t)relations[j].state << relations[j].expiryTime
				<< (int64_t)0 << (uint8_t)0;
		}
		
		Send(p);
	}

	packet p;
	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCFactionRelationListPacket);
	p << false;	// uiRequest
	p << (uint8_t)174;	// numElements

	for (uint16 x = 1000; x < 1174; x++)
	{
		p << relations[x].id << relations[x].id2 << (uint8_t)relations[x].state << relations[x].expiryTime
			<< (int64_t)0 << (uint8_t)0;
	}

	Send(p);

	//size_t numElements = relations.size();
	//const size_t NUM_PER_PACKET = 157;
	//size_t i = 0;

	//while (i < numElements)
	//{
	//	uint8_t packetNumElements = i + NUM_PER_PACKET < numElements ? NUM_PER_PACKET : (uint8_t)(numElements - (i - NUM_PER_PACKET));
	//	packet p(4096U); // reserve 4096 bytes
	//	p.prepareServerResponse(0xdd, 0x01, 0x00, 0x00, (uint16)SC_OpCode::SCFactionRelationListPacket);
	//	p << false;	// uiRequest
	//	p << packetNumElements;
	//
	//	for (size_t j = 0; j < packetNumElements; j++)
	//	{
	//		p << relations[i].id << relations[i].id2 << (uint8_t)relations[i].state << relations[i].expiryTime
	//			<< (int64_t)0 << (uint8_t)0;
	//		i++;
	//	}
	//	this->Send(p);
	//}

	
}

