#pragma once
#include "Opcodes.h"

enum class CLOffsets
{
	CARequestAuthPacket = 0x001,
	CARequestAuthTencentPacket = 0x002,
	CARequestAuthGameOnPacket = 0x003,
	CARequestAuthTrionPacket = 0x004,
	CARequestAuthMailRuPacket = 0x005,
	CAChallengeResponsePacket = 0x006,
	CAChallengeResponse2Packet = 0x007,
	CAOtpNumberPacket = 0x008,
	CAPcCertNumberPacket = 0x00a,
	CAListWorldPacket = 0x00b,
	CAEnterWorldPacket = 0x00c,
	CACancelEnterWorldPacket = 0x00d,
	CARequestReconnectPacket = 0x00e
};

enum class GLOffsets
{
	GLRegisterGameServerPacket = 0x000,
	GLPlayerEnterPacket = 0x001,
	GLPlayerReconnectPacket = 0x002,
	LGRequestInfoPacket = 0x003
};

enum class LCOffsets
{
	ACJoinResponsePacket = 0x000,
	CARequestAuthPacket = 0x001,
	ACChallengePacket = 0x002,
	ACAuthResponsePacket = 0x003,
	ACChallenge2Packet = 0x004,
	ACEnterOtpPacket = 0x005,
	ACShowArsPacket = 0x006,
	ACEnterPcCertPacket = 0x007,
	ACWorldListPacket = 0x008,
	ACWorldQueuePacket = 0x009,
	ACWorldCookiePacket = 0x00a,
	ACEnterWorldDeniedPacket = 0x00b,
	ACLoginDeniedPacket = 0x00c,
	ACAccountWarnedPacket = 0x00d
};

enum class LGOffsets
{
	LGRegisterGameServerPacket = 0x000,
	LGPlayerEnterPacket = 0x001,
	LGPlayerReconnectPacket = 0x002,
	LGRequestInfoPacket = 0x003
};

//class OpcodesLogin : public archecore::network::Opcodes
//{
//public:
//	void BuildOpcodeMap(uint16_t p_Opcode, char const* name);
//	void StoreOpcode();
//};
//
//void OpcodesLogin::BuildOpcodeMap(uint16_t p_Opcode, char const* name)
//{
//
//}
