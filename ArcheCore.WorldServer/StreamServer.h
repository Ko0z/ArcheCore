#pragma once
#include "server_base.h"
#include "StreamSession.h"

class StreamServer : public  server_base
{
public:
	StreamServer(uint16_t nPort) : server_base(nPort) {}

	bool Start()
	{
		try
		{
			// Prepare some work to do before starting the thread so that it doesn't terminate prematurely
			WaitForClientConnection();

			m_threadContext = std::thread([this]() { m_asioContext.run(); });
		}
		catch (std::exception& e)
		{
			// Something prohibited the server from listening
			std::cerr << "[STREAM-SERVER] Exception: " << e.what() << "\n";
			return false;
		}

		std::cout << "[STREAM-SERVER] Started!\n";
		return true;
	}

	// ASYNC - Instructs asio to wait for connection
	void WaitForClientConnection() override
	{
		m_asioAcceptor.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					std::cout << "[STREAM-SERVER] New Connection: " << socket.remote_endpoint() << "\n";

					std::shared_ptr<StreamSession> newconn =
						std::make_shared<StreamSession>(session::owner::server,
							m_asioContext, std::move(socket), m_qMessagesIn);

					// Check if the server should accept this connection
					if (OnClientConnected(newconn))
					{
						// Server accepted connection, store the new connection
						m_deqConnections.push_back(std::move(newconn));

						m_deqConnections.back()->ConnectToClient(nIDCounter++);

						std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved\n";
					}
					else
					{
						std::cout << "[-----] Connection Denied\n";
					}
				}
				else
				{
					std::cout << "[STREAM-SERVER] Connection Error: " << ec.message() << "\n";
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
			entt::registry& world = WorldManager::Instance().GetDefaultWorld();
			msg.remote->ProcessPacket(world, msg.msg);

			nMessageCount++;
		}
	}

protected:
	bool OnClientConnected(std::shared_ptr<StreamSession> client)
	{
		return true;
	}

	void OnClientDisconnected(std::shared_ptr<StreamSession> client)
	{
		std::cout << "[StreamServer] - Client disconnected" << std::endl;
	}

private:
	// Keep one for now..
	entt::registry world;

	std::set<entt::entity> m_PlayerConnections;

};