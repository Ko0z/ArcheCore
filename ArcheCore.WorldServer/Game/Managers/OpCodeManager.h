#pragma once

#include "common_archecore.h"
#include "packet.h"
#include <map>

class WorldSession;

class OpCodes
{
private:
	OpCodes() {}

public:
	static OpCodes& Instance() {
		static OpCodes _Instance;
		return _Instance;
	}

	void BuildOpCodeMap();

	using FunctionPointer = void (WorldSession::*)(packet& packet);

	FunctionPointer GetHandle(int32 opCode, uint8 level);

private:
	void StoreOpCode(int16_t opCode, void (WorldSession::* handle)(packet& recvPacket));

private:
	std::map<int16_t, void (WorldSession::*)(packet& packet)> m_OpCodes;
};