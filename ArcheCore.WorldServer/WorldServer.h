#pragma once
#include "server_base.h"
#include "WorldSession.h"
#include "Managers/WorldManager.h"
#include "Entity/common_entity.h"
#include <Entity/Player.h>
#include <set>
#include <unordered_map>

class WorldServer final : public server_base
{
public:
	WorldServer(uint16_t port) : server_base(port) {
		//world = entt::registry();
	}
	
public:
	// ASYNC - Instructs asio to wait for connection
	void WaitForClientConnection() override
	{
		m_asioAcceptor.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					std::cout << "[WORLD-SERVER] New Connection: " << socket.remote_endpoint() << "\n";

					std::shared_ptr<WorldSession> newconn =
						std::make_shared<WorldSession>(session::owner::server,
							m_asioContext, std::move(socket), m_qMessagesIn);

					// Check if the server should accept this connection
					uint32 connId = OnClientConnected(newconn);

					if (connId > 0)
					{
						// Server accepted connection, store the new connection
						m_deqConnections.push_back(std::move(newconn));

						m_deqConnections.back()->ConnectToClient(connId);

						std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved\n";
					}
					else
					{
						std::cout << "[-----] Connection Denied\n";
					}
				}
				else
				{
					std::cout << "[WORLD-SERVER] Connection Error: " << ec.message() << "\n";
				}

				// Prime the asio context with more work - again simply wait for another connection
				WaitForClientConnection();
			});
	}

	// TODO: Can be handled only in base class, currently kept becuase loginServer is not ready yet
	void Update(size_t nMaxMessages = -1, bool bWait = false) override
	{
		if (bWait) m_qMessagesIn.wait();

		// handle as many messages as specified from nMaxMessages, -1 is all packets
		size_t nMessageCount = 0;
		while (nMessageCount < nMaxMessages && !m_qMessagesIn.empty())
		{
			// Grab the front packet
			auto msg = m_qMessagesIn.pop_front();

			// [DEBUG]
			//std::cout << "Handling Packet..." << std::endl;
			

			// TODO find what registry this connection is part of.
			entt::registry& world = WorldManager::Instance().GetDefaultWorld();

			//msg.remote->ProcessPacket(msg.msg);
			msg.remote->ProcessPacket(world, msg.msg);

			nMessageCount++;
		}
	}

protected:
	
	// If we accept the client connection we send back the id, if its 0, we have declined connection.
	uint32 OnClientConnected(std::shared_ptr<WorldSession> client)
	{
		entt::registry& world = WorldManager::Instance().GetDefaultWorld();

		uint32 connId = nIDCounter++;

		//entt::entity playerEntity = world.create();
		//client->entitySession = playerEntity;
		//world.emplace<e::PlayerSession>(playerEntity, connId, false);

		WorldManager::Instance().AddConnection(connId, client);

		return connId;
	}

	void OnClientDisconnected(std::shared_ptr<WorldSession> client)
	{
		entt::registry& world = WorldManager::Instance().GetDefaultWorld();
		std::cout << "[WorldServer] - Client disconnected" << std::endl;

	}
	
	void UpdateAllWorlds();

};
