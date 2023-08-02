#pragma once
#include "session.h"

class StreamSession : public session
{
public:
	StreamSession(owner _parent, asio::io_context& _asioContext, asio::ip::tcp::socket _socket, tsqueue<owned_packet<session>>& _qIn)
		: session(_parent, _asioContext, std::move(_socket), _qIn)
	{

	}

public:
	void ProcessPacket(entt::registry& reg, packet& packet) override;

	void CTJoinPacket(packet& packet);
	void CTRequestCellPacket(packet& packet);
	void CTContinuePacket(packet& packet);

	void TCJoinResponsePacket(packet& packet, uint8_t response);
	void TCDoodadStreamPacket_Send(packet& p, uint32 instanceId, int32 x, int32 y, int32 requestId);

private:
	int32 RequestIdCounter = 0;
};