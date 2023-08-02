#include "OpCodeManager.h"
#include "common_utils.h"
#include "Opcodes.h"
#include "../WorldSession.h"

#include <spdlog/spdlog.h>
#include "spdlog/fmt/bin_to_hex.h"

void OpCodes::BuildOpCodeMap()
{
	StoreOpCode(CS_OpCode::X2EnterWorldPacket, &WorldSession::X2EnterWorldResponsePacket);
	StoreOpCode(CS_OpCode::CSLeaveWorldPacket, &WorldSession::CSLeaveWorldPacket);
	StoreOpCode(CS_OpCode::CSListCharacterPacket, &WorldSession::CSListCharacterPacket);
	StoreOpCode(CS_OpCode::CSRefreshInCharacterListPacket, &WorldSession::CSRefreshInCharacterListPacket);
	StoreOpCode(CS_OpCode::CSCreateCharacterPacket, &WorldSession::CSCreateCharacterPacket);
	StoreOpCode(CS_OpCode::CSSelectCharacterPacket, &WorldSession::CSCharacterSelectPacket);
	StoreOpCode(CS_OpCode::CSSetLpManageCharacterPacket, &WorldSession::CSSetLpManageCharacterPacket);
	StoreOpCode(CS_OpCode::CSRestrictCheckPacket, &WorldSession::CSRestrictCheckPacket);
	StoreOpCode(CS_OpCode::CSPremiumServieceMsgPacket, &WorldSession::CSPremiumServieceMsgPacket);
	StoreOpCode(CS_OpCode::CSSaveUIDataPacket, &WorldSession::CSSaveUIDataPacket);
	StoreOpCode(CS_OpCode::CSSpawnCharacterPacket, &WorldSession::CSSpawnCharacterPacket);
	StoreOpCode(CS_OpCode::CSNotifyInGamePacket, &WorldSession::CSNotifyInGamePacket);
	StoreOpCode(CS_OpCode::CSMoveUnitPacket, &WorldSession::CSMoveUnitPacket);
	StoreOpCode(CS_OpCode::CSNotifySubZonePacket, &WorldSession::CSNotifySubZonePacket);
	StoreOpCode(CS_OpCode::CSNotifyInGameCompletedPacket, &WorldSession::CSNotifyInGameCompletedPacket);

	// For Proxy packages we use the same HashTable, but with negative OpCode to avoid conflict
	StoreOpCode(-(ProxyOpCode::FinishStatePacket), &WorldSession::HandleFinishStatePacket);
	StoreOpCode(-(ProxyOpCode::PingPacket), &WorldSession::HandlePingPacket);
}

void OpCodes::StoreOpCode(int16_t opCode, void(WorldSession::* handle)(packet& recvPacket))
{
	if (!m_OpCodes.contains(opCode)) {
		m_OpCodes.emplace(opCode, handle);
	}
}

using FunctionPointer = void (WorldSession::*)(packet& packet);
FunctionPointer OpCodes::GetHandle(int32 opCode, uint8 level)
{
	if (level == 1) {
		
		if (m_OpCodes.contains(opCode)) {
			spdlog::info("<-- packet level:{}, opCode: 0x{:03x}", level, opCode);
			return m_OpCodes[opCode];
		}
	}
	if (level == 2) {
		int32 negOpCode = -((int32)opCode);
		if (m_OpCodes.contains(negOpCode)) {
			if (opCode != 0x12) {	// To much spam with ping packet, disable them
				spdlog::info("<-- packet level:{}, opCode: 0x{:03x}", level, opCode);
			}
			return m_OpCodes[negOpCode];
		}
	}
	
	spdlog::warn("<-- UNKNOWN packet level:{}, opCode: 0x{:03x}", level, opCode);
	
	return nullptr;
}