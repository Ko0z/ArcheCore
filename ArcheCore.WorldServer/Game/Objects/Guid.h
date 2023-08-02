#pragma once
#include "common_archecore.h"
/*
enum HighGuid
{
	HIGHGUID_ITEM = 0x4000,                       // blizz 4000
	HIGHGUID_CONTAINER = 0x4000,                       // blizz 4000
	HIGHGUID_PLAYER = 0x0000,                       // blizz 0000
	HIGHGUID_GAMEOBJECT = 0xF110,                       // blizz F110
	HIGHGUID_TRANSPORT = 0xF120,                       // blizz F120 (for GAMEOBJECT_TYPE_TRANSPORT)
	HIGHGUID_UNIT = 0xF130,                       // blizz F130
	HIGHGUID_PET = 0xF140,                       // blizz F140
	HIGHGUID_DYNAMICOBJECT = 0xF100,                       // blizz F100
	HIGHGUID_CORPSE = 0xF101,                       // blizz F100
	HIGHGUID_MO_TRANSPORT = 0x1FC0,                       // blizz 1FC0 (for GAMEOBJECT_TYPE_MO_TRANSPORT)
};

class Guid 
{
public:
	Guid() : m_Guid(0) {}

public:
	void Set(const uint64& guid);
	void Clear() { m_Guid = 0; }

public:
	uint64 const& GetRawValue() const { return m_Guid; }

	static HighGuid GetHigh(uint64 guid) { return HighGuid((guid >> 48) & 0x0000FFFF); }
	HighGuid GetHigh() const { return GetHigh(m_Guid); }

	bool IsEmpty() const { return m_Guid == 0; }

private:
	uint64 m_Guid = 0;
};
*/