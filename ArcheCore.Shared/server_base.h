/// Thanks to OneLoneCoder, javidx9 on youtube, for setting the founding blocks of the message handling.

#pragma once
#include "net_common.h"
#include "packet.h"
#include "net_tsqueue.h"
#include "session.h"

using archecore::network::tsqueue;

class server_base
{
public:
	server_base(uint16_t port)
		: m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	{
		
	}

	virtual ~server_base()
	{
		Stop();
	}

	bool Start()
	{
		//if (SetupDatabase() == false)
		//{
		//	std::cerr << "[DATABASE] Setup error." << std::endl;
		//	return false;
		//}
		
		try
		{
			// Prepare some work to do before starting the thread so that it doesn't terminate prematurely
			WaitForClientConnection();

			m_threadContext = std::thread([this]() { m_asioContext.run(); });
		}
		catch (std::exception& e)
		{
			// Something prohibited the server from listening
			std::cerr << "[SERVER] Exception: " << e.what() << "\n";
			return false;
		}

		return true;
	}

	void Stop()
	{
		m_asioContext.stop();

		// Wait on the thread and close it
		if (m_threadContext.joinable()) m_threadContext.join();

		std::cout << "[SERVER] Stopped!\n";
	}

	// ASYNC - Instructs asio to wait for connection
	virtual void WaitForClientConnection()
	{
		m_asioAcceptor.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << "\n";
					
					std::shared_ptr<session> newconn =
						std::make_shared<session>(session::owner::server,
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
					std::cout << "[SERVER] Connection Error: " << ec.message() << "\n";
				}

				// Prime the asio context with more work - again simply wait for another connection
				WaitForClientConnection();
			});
	}

	virtual void Update(size_t nMaxMessages = -1, bool bWait = false)
	{
		if (bWait) m_qMessagesIn.wait();

		// handle as many messages as specified from nMaxMessages, -1 is all packets
		size_t nMessageCount = 0;
		while (nMessageCount < nMaxMessages && !m_qMessagesIn.empty())
		{
			// Grab the front packet
			auto msg = m_qMessagesIn.pop_front();

			// pass to packet handler
			// TODO: Maybe its better to handle packets in the "session class"
			//msg.remote->OnPacket();
			//std::cout << "Handling Packet..." << std::endl;
			OnMessage(msg.remote, msg.msg);

			nMessageCount++;
		}
	}

protected:
	// Called when a client connects, you can veto the connection by returning false
	virtual bool OnClientConnected(std::shared_ptr<session> client)
	{
		return false;
	}

	// Called when a client appeards to have disconnected
	virtual void OnClientDisconnected(std::shared_ptr<session> client)
	{

	}

	// Called when a packet arrives
	virtual void OnMessage(std::shared_ptr<session> client, packet& msg)
	{

	}

protected:
	// Thread safe queue for incoming packet packets
	tsqueue<owned_packet<session>> m_qMessagesIn;


	/// Container of active validated connections
	std::deque<std::shared_ptr<session>> m_deqConnections;

	// Order of declaration is important, it is also the order of initialization
	asio::io_context m_asioContext;
	std::thread m_threadContext;

	// These things need an asio context
	asio::ip::tcp::acceptor m_asioAcceptor;

	// Clients will be identified in the "wider system" via an ID
	uint32_t nIDCounter = 10000;
};