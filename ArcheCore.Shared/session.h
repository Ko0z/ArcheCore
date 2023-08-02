/// Thanks to OneLoneCoder, javidx9 on youtube, for setting the founding blocks of the message handling.

#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "packet.h"
#include "common_utils.h"

#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>
//#include "ByteBuffer.h"

using archecore::network::tsqueue;
// New Impl

class session : public std::enable_shared_from_this<session>
{
public:
	enum class owner
	{
		server,
		client
	};

	session(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_packet<session>>& qIn)
		: m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn)
	{
		m_nOwnerType = parent;
	}

	virtual ~session() {}

	uint32_t GetID() const
	{
		return id;
	}

public:
	void ConnectToClient(uint32_t uid = 0)
	{
		if (m_nOwnerType == owner::server)
		{
			if (m_socket.is_open())
			{
				id = uid;
				ReadHeader();
			}
		}
	}

	void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
	{
		// Only clients can connect to servers
		if (m_nOwnerType == owner::client)
		{
			// Request asio attemps to connect to an endpoint
			asio::async_connect(m_socket, endpoints,
				[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
				{
					if (!ec)
					{
						ReadHeader();
					}
				});
		}
	}

	void Disconnect()
	{
		if (IsConnected())
		{
			asio::post(m_asioContext, [this]() { m_socket.close(); });
		}
	}

	bool IsConnected() const
	{
		return m_socket.is_open();
	}

public:
	virtual void Send(packet& packet)
	{
		assert(packet.body.size() < USHRT_MAX);
		packet.header.size = (uint16_t)packet.body.size();
		packet.wpos(0);
		packet << (uint16_t)(packet.body.size() - 2);

		SendPacket(packet);
	}

	virtual void ProcessPacket(entt::registry& reg, packet& packet) {}

	// ASYNC - Send a packet, connections are one-to-one so no need to specifiy
	// the target, for a client, the target is the server and vice versa
private:
	void SendPacket(const packet& p)
	{
		asio::post(m_asioContext,
			[this, p]()
			{
				// If the queue has a packet in it, then we must 
				// assume that it is in the process of asynchronously being written.
				// Either way add the packet to the queue to be output. If no messages
				// were available to be written, then start the process of writing the
				// packet at the front of the queue.
				bool bWritingMessage = !m_qMessagesOut.empty();
				// [DEBUG]
				//std::cout << "Prep Push Packet, In Queue: " << m_qMessagesOut.count() << "\n";

				m_qMessagesOut.push_back(p);
				if (!bWritingMessage)
				{
					//WriteHeader();
					PushPacket();
				}
			});
	}

protected:
	// ASYNC - Prime context ready to read a packet header
	virtual void ReadHeader()
	{
		// If this function is called, we are expecting asio to wait until it receives
		// enough bytes to form a header of a packet. We know the headers are a fixed
		// size, so allocate a transmission buffer large enough to store it. In fact, 
		// we will construct the packet in a "temporary" packet object as it's 
		// convenient to work with.
		asio::async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(packet_header)),
			[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					/// IMPORTANT: If the client send a packet that has "many smaller packets inside", 
					/// they will be correctly handled and sent to this sessions ProcessPacket function one by one.
					/// 
					/// DO NOT INVESTIGATE AGAIN.

					// A complete packet header has been read, check if this packet
					// has a body to follow...
					if (m_msgTemporaryIn.header.size > 0)
					{
						// ...it does, so allocate enough space in the messages' body
						// vector, and issue asio with the task to read the body.
						m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
						ReadBody();
					}
					else
					{
						// it doesn't, so add this bodyless packet to the connections
						// incoming packet queue
						AddToIncomingMessageQueue();
					}
					//m_byteBuffeer = NULL;
				}
				else
				{
					// Reading from the client went wrong, most likely a disconnect
					// has occurred. Close the socket and let the system tidy it up later.
					std::cout << "[" << id << "] Read Header Fail.\n";
					std::cout << ec.message() << std::endl;
					m_socket.close();
				}
			});
	}

	// ASYNC - Prime context ready to read a packet body
	virtual void ReadBody()
	{
		asio::async_read(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
			[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					// If we have a body of at least 6 bytes, we can populate our packet variables
					if (m_msgTemporaryIn.body.size() >= sizeof(uint16_t))
					{
						// [DEBUG]
						//std::cout << "Packet In:" << std::endl;
						//std::cout << m_msgTemporaryIn;

						memcpy(&m_msgTemporaryIn.opcode, &m_msgTemporaryIn.body[0], sizeof(uint16_t));

						AddToIncomingMessageQueue();
					}

				}
				else
				{
					std::cout << "[" << id << "] Read Body Fail.\n";
					std::cout << ec.message() << std::endl;
					m_socket.close();
				}
			});
	}

	virtual void PushPacket()
	{
		asio::async_write(m_socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
			[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					// [DEBUG]
					//std::cout << "Body:" << std::endl;
					//std::cout << m_qMessagesOut.front();

					// done with the packet, pop it
					m_qMessagesOut.pop_front();

					// If we still have more bodies to write, continue
					if (!m_qMessagesOut.empty())
					{
						PushPacket();
					}
				}
				else
				{
					std::cout << "[" << id << "]" << "OpCode: " << m_qMessagesOut.front().opcode << " - Write Packet Fail.\n";
					std::cout << ec.message() << std::endl;
					m_socket.close();
				}
			});
	}

	/// TODO: Make it possible to pack in many smaller bodies into one packet before sending it off,
	/// like the client does sometimes with many smaller bodies in the same packet.
	// ASYNC - Prime context ready to write a packet header
	virtual void WriteHeader()
	{
		// If called, we know the outgoing packet queue have at least one msg to send,
		// so allocate a transmission buffer to hold the msg, and issue the work - asio, send these bytes
		asio::async_write(m_socket, asio::buffer(&m_qMessagesOut.front().header.size, sizeof(uint16_t)),
			[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					if (m_qMessagesOut.front().body.size() > 0)
					{
						// [DEBUG]
						//std::cout << "Pushing Packet of Size: ";
						//std::cout << m_qMessagesOut.front().header.size << std::endl;
						WriteBody();
					}
					else
					{
						// [DEBUG]
						//std::cout << "Pushing Empty Packet:" << std::endl;
						m_qMessagesOut.pop_front();

						if (!m_qMessagesOut.empty())
						{
							WriteHeader();
						}
					}
				}
				else
				{
					std::cout << "[" << id << "] Write Header Fail.\n";
					std::cout << ec.message() << std::endl;
					m_socket.close();
				}
			});
	}

	// ASYNC - Prime context ready to write a packet body
	virtual void WriteBody()
	{
		// If called, a header has just been sent, and that header indicated a body existed for this msg.
		// Fill a transmission buffer with the body data and send it
		asio::async_write(m_socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
			[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					// [DEBUG]
					//std::cout << "Body:" << std::endl;
					//std::cout << m_qMessagesOut.front();

					// done with the packet, pop it
					m_qMessagesOut.pop_front();

					// If we still have more bodies to write, continue
					if (!m_qMessagesOut.empty())
					{
						WriteHeader();
					}
				}
				else
				{
					std::cout << "[" << id << "] Write Body Fail.\n";
					std::cout << ec.message() << std::endl;
					m_socket.close();
				}
			});
	}

	void AddToIncomingMessageQueue()
	{
		if (m_nOwnerType == owner::server)
			m_qMessagesIn.push_back({ this->shared_from_this(), m_msgTemporaryIn });
		else
			m_qMessagesIn.push_back({ nullptr, m_msgTemporaryIn });

		ReadHeader();
	}

protected:
	// Each session has a unique socket to a remote
	asio::ip::tcp::socket m_socket;

	// This context is shared with the whole asio instance
	asio::io_context& m_asioContext;

	// This queue holds all messages to be sent to the remote side of session
	tsqueue<packet> m_qMessagesOut;

	// Not used yet..
	//std::chrono::time_point<std::chrono::milliseconds> m_LastPing;
	
	// This queue holds all messages that have been recieved from the remote
	// side of this session. Note it is a reference as the "owner" of this 
	// session is expected to provide a queue
	tsqueue<owned_packet<session>>& m_qMessagesIn;

	packet m_msgTemporaryIn;
	// The "owner" decides how some of the session behaves
	owner m_nOwnerType = owner::server;

	uint32_t id = 0;

};
