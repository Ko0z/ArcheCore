#include "server_base.h"
#include <iomanip>
#include "OpcodesLogin.h"
#include <pugixml.hpp>
#include <common_archecore.h>

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Needed for sqlite database error handling..
static int callback(void* NotUsed, int argc, char** argv, char** azColName)
{
	int i;
	for (i = 0; i < argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

class LoginServer : public server_base
{
public:
	LoginServer(uint16_t nPort) : server_base(nPort)
	{

	}
protected:
	virtual bool OnClientConnected(std::shared_ptr<session> client)
	{
		return true;
	}

	virtual void OnClientDisconnected(std::shared_ptr<session> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	virtual void OnMessage(std::shared_ptr<session> client, packet& packet)
	{
		// Check Opcode and call appropriate action
		packet.resetReaderWriter();

		switch (packet.opcode)
		{
		case (uint16_t)CLOffsets::CARequestAuthTrionPacket:
			std::cout << "->CARequestAuthTrionPacket" << std::endl;
			HandleCARequestAuthPacket(client, packet);
			break;
		case(uint16_t)CLOffsets::CAListWorldPacket:
			std::cout << "->CAListWorldPacket" << std::endl;
			HandleCAListWorldPacket(client, packet);
			break;
		case(uint16_t)CLOffsets::CAEnterWorldPacket:
			std::cout << "->CAEnterWorldPacket" << std::endl;
			HandleCAEnterWorldPacket(client, packet);
			break;
		default:
			break;
		}
	}

	void HandleCARequestAuthPacket(std::shared_ptr<session> client, packet& msg);
	
	void HandleCAListWorldPacket(std::shared_ptr<session> client, packet& msg);

	void HandleCAEnterWorldPacket(std::shared_ptr<session> client, packet& msg);

	void ACWorldCookiePacket(std::shared_ptr<session> client, packet& packet);

	void Login(std::string& username, std::vector<char>& password)
	{
		
	}

public:
	std::string ServerIp = "";
	uint32_t ServerPort = 0;

};

void LoginServer::HandleCARequestAuthPacket(std::shared_ptr<session> client, packet& msg)
{
	std::cout << "   Handling packet..." << std::endl;
	
	// Extract the opcode at the start of packet
	uint16_t opcode;	msg >> opcode;
	// Extract all data one by one from the packet
	uint32_t pFrom;		msg >> pFrom;
	uint32_t pTo;		msg >> pTo;
	uint8_t dev;		msg >> dev;
	uint16_t macSize;	msg >> macSize;
	std::string mac;	msg.read_bytes(mac, macSize);
	uint16_t ticketSize; msg >> ticketSize;
	std::string ticket;	msg.read_bytes(ticket, ticketSize);
	uint16_t sigSize;	msg >> sigSize;
	std::string signature;	msg.read_bytes(signature, sigSize);
	uint8_t isLast;		msg >> isLast;

	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_string((char*)ticket.c_str());
	if (!res)
	{
		std::cout << "Parse error: " << res.description()
			<< ", character pos= " << res.offset;
		return;
	}
	else
	{
		// A valid XML document must have a single root node
		pugi::xml_node root = doc.root();

		std::string username = root.child("authTicket").child("username").text().get();
		std::string password = root.child("authTicket").child("password").text().get();
		std::cout << "usernameString: " << username << std::endl;
		std::cout << "passwordString: " << password << std::endl;

		if (username.empty() || password.empty())
		{
			std::cerr << "Username Empty!" << std::endl;
			std::cerr << "Password Empty!" << std::endl;
		}

		std::vector<char> passwordBytes = msg.HexToBytes(password);
	}

	// Print helper values of data
	std::cout << std::hex << std::setfill('0') << std::setw(sizeof(pFrom)) << pFrom << "\n";
	std::cout << std::hex << std::setfill('0') << std::setw(sizeof(pTo)) << pTo << "\n";
	std::cout << std::hex << std::setfill('0') << std::setw(sizeof(dev)) << dev << "\n";
	std::cout << std::hex << std::setfill('0') << std::setw(sizeof(macSize)) << "Size: " << macSize << "\n";
	msg.printStringAsHex(mac);
	std::cout << "rPos: " << msg.rpos() << std::endl; std::cout << std::hex << std::setfill('0') << std::setw(sizeof(ticketSize)) << ticketSize << "\n";
	msg.printStringAsHex(ticket);
	std::cout << std::hex << std::setfill('0') << std::setw(sizeof(sigSize)) << sigSize << "\n";
	msg.printStringAsHex(signature);
	std::cout << std::hex << std::setfill('0') << std::setw(sizeof(isLast)) << isLast << "\n";

	std::cout << std::dec << "...Packet Handled." << "\n";
	msg.body.clear();
	
	//msg.HexToBytesAppend(std::string("0C00000000000300000000000000"));
	//client->SendPacket(msg);
	// Fix this shit
	uint16_t opCodeFirst = 0x000;
	uint16_t reason = 0;
	uint64_t afs = 6;
	msg << opCodeFirst << reason << afs;
	//msg.HexToBytesAppend(std::string("0000 0000 03 00 00 00 00 00 00 00"));
	client->Send(msg);

	packet secondPacket;
	secondPacket.body.clear();
	secondPacket.body.reserve(41);
	//secondPacket.body.
	//uint16_t size = 41;
	uint16_t opCode = 0x003;
	//uint32_t accountID = 0x00005833;
	uint32_t accountID = client.get()->GetID();
	//uint16_t wskSize = 32;
	const char* wsk = "00000000000000000000000000000000";
	uint8_t slotCount = 0;

	//secondPacket << size << opCode << accountID << wskSize << wsk << slotCount;
	secondPacket << opCode << accountID;
	secondPacket.Write(wsk);
	secondPacket << slotCount;
	//secondPacket.body.shrink_to_fit();
	//client->SendPacket(secondPacket);
	client->Send(secondPacket);
}

void LoginServer::HandleCAListWorldPacket(std::shared_ptr<session> client, packet& packet)
{
	packet.body.clear();
	uint16_t opCode = 0x08;
	uint8_t serverCount = 1;
	uint8_t serverID = 1;	
	//uint16_t stringBufferSize = 10;
	const char* serverName = "Kyprosa";
	uint8_t serverOnline = true;
	uint8_t serverLoad = 0;		// 0 = Low, 1 = Medium, 2 = High
	
	packet << opCode << serverCount << serverID << serverName << serverOnline << serverLoad;
	
	//packet << (uint16_t)0 << (uint16_t)0;
	//packet << opCode << serverCount << serverID << serverName << serverOnline << serverLoad;

	for (size_t i = 0; i < 9; i++)
	{
		packet << (uint8_t)0;
	}
	
	uint8_t characterCount = 0;
	packet << characterCount;
	
	
	client->Send(packet);
}

// This packet is sent to the interal game world server and not to the client
void LoginServer::HandleCAEnterWorldPacket(std::shared_ptr<session> client, packet& packet)
{
	// Temporary test
	ACWorldCookiePacket(client, packet);

	// SAVED, send internally to GAME WORLD SERVER, whom in turn send to client Cookie
}

void LoginServer::ACWorldCookiePacket(std::shared_ptr<session> client, packet& packet)
{
	uint16_t opCode = 0x00a;
	uint32_t cookieID = client.get()->GetID();
	uint16_t bufferSize = 4;
	
	uint32_t ipAddressParsedReverse = archecore::net::GetParsedIpAddressReverse(ServerIp.c_str());
	
	packet.body.clear();
	packet << opCode << cookieID;

	for (size_t i = 0; i < 4; i++)
	{
		packet << ipAddressParsedReverse << (uint16_t)ServerPort;
	}
	
	client->Send(packet);
}

int main()
{
	std::filesystem::path path = std::filesystem::current_path();
	std::filesystem::path full_path = std::filesystem::absolute(path);
	// Will make the path string to platform specific format?
	std::filesystem::path preferred_path = full_path.make_preferred();
	std::string real_path_string = preferred_path.string();

	std::replace(real_path_string.begin(), real_path_string.end(), '\\', '/');
	std::string config_string = "/Server.cfg";

	std::string config_path = real_path_string + config_string;

	std::cout << "Login Server Config Path: " << config_path << std::endl;

	// JSON stuff
	std::ifstream f((config_path));
	json data = json::parse(f);
	if (data.empty()) {
		std::cout << "JSON ERROR: data is empty!" << std::endl;
		std::cin.get();
		return 0;
	}

	int loginServerPort = data["LOGIN-SERVER-PORT"];
	LoginServer loginServer(loginServerPort);

	loginServer.ServerIp = data["GAME-SERVER-IP"];
	loginServer.ServerPort = data["GAME-SERVER-PORT"];
	
	std::cout << "Login Server listening on Port: " << loginServerPort << std::endl;
	std::cout << "Game Server IP: " << loginServer.ServerIp << std::endl;
	std::cout << "Game Server Port: " << loginServer.ServerPort << std::endl;


	if (loginServer.Start())
	{
		std::cout << "[LOGIN-SERVER] Started!\n";
		while (1)
		{
			loginServer.Update(-1, true);
		}
	}

	return 0;
}