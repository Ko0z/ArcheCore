#pragma once

#include "common_archecore.h"
#include "session.h"
#include "Character/CharacterStructs.h"
#include "SystemFaction.h"
#include "Managers/CharacterManager.h"
#include "Managers/WorldManager.h"
#include "Entity/common_entity.h"
#include "Zones/ZoneConflict.h"

#include <memory>

struct e::MovementPacketData;

class WorldSession : public session
{	
public:
	//friend class CharacterManager;
	WorldSession(owner _parent, asio::io_context& _asioContext, asio::ip::tcp::socket _socket, tsqueue<owned_packet<session>>& _qIn)
		: session(_parent, _asioContext, std::move(_socket), _qIn)
	{
		
	}

	~WorldSession() {
		spdlog::warn("Connection lost with id: {}", id);

		WorldManager::Instance().NotifyConnectionLost(id);
	}

public:
	void ProcessPacket(entt::registry& reg, packet& packet) override;
	
	void BroadcastMessage(packet& p, std::vector<std::shared_ptr<WorldSession>>& connections, bool includeSelf);

public:	/// Packet handling functions
	void Handle_NULL(packet& packet);
	// OpCode Handlers...	Client -> WorldServer
	void X2EnterWorldResponsePacket(packet& packet);		// 0x000
	void CSLeaveWorldPacket(packet& packet);				// 0x001
	void CSListCharacterPacket(packet& packet);				// 0x020
	void CSRefreshInCharacterListPacket(packet& packet);	// 0x021
	void CSPremiumServieceMsgPacket(packet& packet);		// 0x13c
	void CSCreateCharacterPacket(packet& p);				// 0x022
	void CSRestrictCheckPacket(packet& p);
	void CSCharacterSelectPacket(packet& p);
	void CSSetLpManageCharacterPacket(packet& p);
	void CSSaveUIDataPacket(packet& p);
	void CSSpawnCharacterPacket(packet& p);
	void CSNotifyInGamePacket(packet& p);
	void CSMoveUnitPacket(packet& p);
	void CSNotifySubZonePacket(packet& p);
	void CSNotifyInGameCompletedPacket(packet& p);

	// Proxy packets
	void ChangeStatePacket(int32_t state);				
	void HandleFinishStatePacket(packet& packet);
	void HandlePingPacket(packet& packet);
	void SetGameTypePacket(const char* level, uint64_t checksum, uint8_t immersive);

	// G2C, - WorldServer -> Client
	void SCInitialConfigPacket();
	void SCTrionConfigPacket(bool activate);
	void SCAccountInfoPacket(int32_t payMethod, int32_t payLocation, int64_t payStart, int64_t payEnd);
	void SCChatSpamDelayPacket();
	void SCAccountAttributeConfigPacket();
	void SCLevelRestrictionConfigPacket(uint8_t searchLevel, uint8_t bidLevel, uint8_t postLevel, uint8_t trade, uint8_t mail);
	void SCGetSlotCountPacket(uint8_t sc);
	void SCRaceCongestionPacket();
	void SCCharacterListPacket(bool last);
	void SCRefreshInCharacterListPacket(packet& packet);						// 0x1cd
	void SCAccountWarnedPacket(packet& p, uint8_t source, const char* text);	// 0x1a1
	void SCCharacterCreationFailedPacket(uint8_t reason, packet& p);			// 0x03d
	void SCFactionListPacket();				// 0x008
	void SCFactionRelationListPacket(std::vector<FactionRelation>& relations);
	void SCUpdatePremiumPointPacket(int32_t point, uint8_t oldPg, uint8_t pG);
	void SCCreateCharacterResponsePacket(packet& p);
	void SCResultRestrictCheckPacket(packet& p, uint32 characterId, uint8 code, uint8 result);
	void SCLpManagedPacket(packet& p, uint32 characterId);
	void SCCharacterStatePacket(packet& p, entt::registry& world);
	void SCCharacterGamePointsPacket(packet& p, entt::registry& world);
	void SCCharacterInvenInitPacket(packet& p, const e::InventoryBag& bag, const e::InventoryWarehouse& bank) const;
	void SCCharacterInvenContentsPacket(packet& p, uint8 startIndex, entt::registry& world, const e::CharacterInventory& inv, e::SlotType invType) const;
	void SCActionSlotsPacket_Send(e::ActionSlots& actionSlotsComponent);
	void SCQuestsPacket_Send();
	void SCCompletedQuestsPacket_Send();
	void SCActabilityPacket_Send(e::CharacterActability& actabilities, bool last);
	void SCCountUnreadMailPacket_Send();
	void SCAppellationsPacket_Send(e::CharacterAppellations& appellations);
	void SCConflictZoneStatePacket_Send(uint16 id, ZoneConflictType conflictType, time_t endTime);
	void SCExpeditionRolePolicyListPacket_Send();
	void SCResponseUIDataPacket_Send(uint32 characterId, uint16 uiDataType, std::string& uiData);
	void SCJoinedChatChannelPacket(e::Faction& factionComponent);
	void SCUnitStatePacket(e::BaseUnitType unitType, entt::entity entity, bool toSelf = true);
	void SCDetailedTimeOfDayPacket();
	void SCOneUnitMovementPacket(uint32 objId, e::MovementPacketData& moveData, bool includeSelf);
public:
	//entt::entity entitySession = entt::null;
	
	/// <summary>
	/// The session does not own the entity character, it is just an ID <para/>
	/// The world registry is the owner, so even if this connection die, the character <para/>
	/// will NOT get removed instantly.
	/// </summary>
	entt::entity entityCharacter = entt::null;

	/// <summary>
	/// Keep track of what world this connection and its character is currently in?
	/// </summary>
	uint32 WorldId = 0;
	bool inGame = false;
	/// <summary>
	/// An account can have 4 characters, store them here temp...
	/// </summary>
	std::array<uint32, 4> characterIds {};
};
