#include "StreamSession.h"
#include "common_utils.h"
#include "Opcodes.h"
#include <spdlog/spdlog.h>

void StreamSession::ProcessPacket(entt::registry& reg, packet& packet)
{
	packet.resetReaderWriter();

	uint16_t opCode; packet >> opCode;
	packet.opcode = opCode;

	spdlog::info("<-- packet opCode: 0x{:03x} [StreamServer]", opCode);

	switch (opCode)
	{
	case 0x001:
		CTJoinPacket(packet);
		break;
	case (uint16)InStreamOpCode::CTRequestCellPacket:
		//std::cout << "[StreamServer] - CTRequestCellPacket " << std::endl;
		CTRequestCellPacket(packet);
		break;
	case (uint16)InStreamOpCode::CTContinuePacket:
		//std::cout << "[StreamServer] - CTContinuePacket " << std::endl;
		//CTContinuePacket(packet);
		break;
	default:
		spdlog::warn("<-- UNKNOWN packet opCode: 0x{:03x} [StreamServer] ", opCode);
		break;
	}
}

void StreamSession::CTJoinPacket(packet& packet)
{
	uint32_t accountID; packet >> accountID;
	uint32_t cookie; packet >> cookie;

	// Check validity... just accepting for now
	TCJoinResponsePacket(packet, 0);
}

void StreamSession::CTRequestCellPacket(packet& packet)
{
	uint32 instanceId;
	int32 x;
	int32 y;

	packet >> instanceId >> x >> y;

	spdlog::info("[StreamServer] CTRequestCellPacket: #.{} ({},{})", instanceId, x, y);

	TCDoodadStreamPacket_Send(packet, instanceId, x, y, -1);
}

void StreamSession::CTContinuePacket(packet& packet)
{
	/*
	int32 id = 0;
	int32 next = 0;

	packet >> id >> next;
	spdlog::info("[StreamServer] CTContinuePacket: id: {} next: {}", id, next);
	*/

	//TCDoodadStreamPacket_Send(packet, 1, 0, 0, id);
}

void StreamSession::TCJoinResponsePacket(packet& packet, uint8_t response)
{
	//uint16_t opCode = 0x001;
	//packet.body.clear();
	//packet.resetReaderWriter();

	packet.prepareStreamServerResponse((uint16)OutStreamOpCode::TCJoinResponsePacket);

	packet << response;
	
	this->Send(packet);
}

void StreamSession::TCDoodadStreamPacket_Send(packet& p, uint32 instanceId, int32 x, int32 y, int32 requestId)
{
	//uint16_t opCode = (uint16)OutStreamOpCode::TCDoodadStreamPacket;
	//p.body.clear();
	//p.resetReaderWriter();
	//
	//p << opCode;

	p.prepareStreamServerResponse((uint16)OutStreamOpCode::TCDoodadStreamPacket);

	//int32 requestId = RequestIdCounter++;
	if (requestId < 0) {
		requestId = 0;
	}
	int32 next = 0;
	//int32 next = RequestIdCounter++;
	//if (next > 25) {
	//	return;
	//}
	int32 count = 0;

	p << requestId << next << count;

	this->Send(p);
}
