/// Thanks to OneLoneCoder, javidx9 on youtube, for setting the founding blocks of the message handling.

#pragma once

#include <iomanip>
#include <cassert>
#include <ostream>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <glm/vec3.hpp>
#include "common_utils.h"
// New Impl

struct packet_header
{
	uint16_t size = 0;			// 2 byte
};

struct packet
{
	packet_header header{};
	std::vector<uint8_t> body;
	uint16_t opcode = 0x00;	// Just a helper for us when debug log etc... We send the opCode in the body of the msg after size.

	packet() {
		// The idea is that the majority of packets dont need more than 128 bytes. And the few packets that do
		// can dynamically increase the size of the vector if needed, to help minimize mem operations
		body.reserve(128);
	}

	packet(uint8_t reserveSize)
	{
		body.reserve(reserveSize);
	}

	packet(uint32_t reserveSize) 
	{ 
		body.reserve(reserveSize); 
	}

	const uint8_t* contents() const { return &body[0]; }

	size_t size() const { return body.size(); }

	bool empty() const { return body.empty(); }

	// 0xdd is server
	void prepareServerResponse(uint8_t isServer, uint8_t level, uint8_t hash, uint8_t count, uint16_t opCode)
	{
		//std::cout << "Prep opCode: ";
		//archecore::utils::PrintHex(opCode);
		opcode = opCode;

		spdlog::info("--> packet level:{}, opCode: 0x{:03x}", level, opCode);

		body.clear();
		(*this) << isServer << level << hash << count << opCode;
	}

	// 0xdd is server
	void prepareServerResponse(uint8_t isServer, uint8_t level, uint16_t opCode)
	{
		body.clear();
		(*this) << isServer << level << opCode;
	}

	void prepareStreamServerResponse(uint16 opCode) {
		spdlog::info("--> packet opCode: 0x{:03x} [StreamServer]", opCode);

		body.clear();
		(*this) << opCode;
	}

	//void resetPacket()
	//{
	//	body.clear();
	//	resetReaderWriter();
	//}

	void resetReaderWriter()
	{
		_rpos = 0;
		_wpos = 2;	// We start writing on index 2 to leave the first 2 bytes for the uint16_t that represent packet size
	}

	friend std::ostream& operator << (std::ostream& os, const packet& msg)
	{
		std::ostringstream ss;
		size_t j = 1, k = 1;
		ss << "         ";
		for (size_t i = 0; i < msg.body.size(); ++i)
		{
			if ((i == (j * 8)) && ((i != (k * 16))))
			{
				ss << "| ";
				++j;
			}
			else if (i == (k * 16))
			{
				ss << "\n";

				//if (sLog.IsIncludeTime())
				ss << "         ";

				++k;
				++j;
			}

			char buf[4];
			snprintf(buf, 4, "%02X", *((uint8_t const*)&msg.body[i]));
			ss << buf << " ";

		}
		os << ss.str().c_str() << std::endl;
		return os;
	}

	void printStringAsHex(std::string& s)
	{
		std::ostringstream ss;
		size_t j = 1, k = 1;
		ss << "         ";
		for (size_t i = 0; i < s.size(); ++i)
		{
			if ((i == (j * 8)) && ((i != (k * 16))))
			{
				ss << "| ";
				++j;
			}
			else if (i == (k * 16))
			{
				ss << "\n";

				//if (sLog.IsIncludeTime())
				ss << "         ";

				++k;
				++j;
			}

			char buf[4];
			snprintf(buf, 4, "%02X", *((uint8_t const*)&s[i]));
			ss << buf << " ";
		}
		std::cout << ss.str().c_str() << std::endl;
	}

	//void ReadFromEnd(uint8_t* dest, size_t length)
	//{
	//	if (length > body.size())
	//	{
	//		std::cerr << "Read buffer error, len: " << length << " size: " << body.size() << "\n";
	//		return;
	//	}
	//	size_t i = body.size() - length;
	//	memcpy(dest, body.data() + i, length);
	//	body.resize(i);
	//}
	//
	//
	//
	//void Read(uint8_t* dest, size_t length)
	//{
	//	if (length > body.size())
	//	{
	//		std::cerr << "Read buffer error, len: " << length << " size: " << body.size() << "\n";
	//		return;
	//	}
	//	size_t i = body.size() - length;
	//	memcpy(dest, &body[0], length);
	//	body.resize(i);
	//}

	void ReadBc(uint32& value) {
		if (_rpos + 3 > size()) {
			spdlog::error("Tried to read further than packet body size.");
			return;
		}
		uint16 a;
		uint8 b;

		(*this) >> a >> b;

		value = a + (b << 16);
	}

	/// <summary>
	/// Writes a uint32 formed from the least significant bytes of the specified value.
	/// This is for little endian.
	/// </summary>
	/// <param name="value">Value to write</param>
	/// <param name="numBytes">How many bytes to write</param>
	void WriteBc(uint32 value) {
		
		for (uint8 i = 0; i < 3; i++)
		{
			(*this) << (uint8)(value & 0xff);
			value = value >> 8;
		}
	}

	// TODO: move to utility class, this is not necessarily packet specific
	std::vector<char> HexToBytes(const std::string& hex)
	{
		std::vector<char> bytes;

		for (unsigned int i = 0; i < hex.length(); i += 2)
		{
			std::string byteString = hex.substr(i, 2);
			char byte = (char)strtol(byteString.c_str(), NULL, 16);
			bytes.push_back(byte);
		}

		return bytes;
	}

	// TODO: move to utility class, this is not necessarily packet specific
	void HexToBytes(const std::string& hexIn, std::string& dest)
	{
		for (unsigned int i = 0; i < hexIn.length(); i += 2)
		{
			std::string byteString = hexIn.substr(i, 2);
			char byte = (char)strtol(byteString.c_str(), NULL, 16);
			dest.push_back(byte);
		}
	}

	void HexToBytesAppend(const std::string& hex)
	{
		//std::vector<char> bytes;

		for (unsigned int i = 0; i < hex.length(); i += 2)
		{
			std::string byteString = hex.substr(i, 2);
			char byte = (char)strtol(byteString.c_str(), NULL, 16);
			//body.push_back(byte);
			append<char>(byte);
		}

		//return bytes;
	}

	//	[INSERT TO PACKET]
	packet& operator<<(uint8_t value)
	{
		append<uint8_t>(value);
		return *this;
	}

	packet& operator<<(uint16_t value)
	{
		append<uint16_t>(value);
		return *this;
	}

	packet& operator<<(uint32_t value)
	{
		append<uint32_t>(value);
		return *this;
	}

	packet& operator<<(uint64_t value)
	{
		append<uint64_t>(value);
		return *this;
	}

	// signed as in 2e complement
	packet& operator<<(int8_t value)
	{
		append<int8_t>(value);
		return *this;
	}

	packet& operator<<(int16_t value)
	{
		append<int16_t>(value);
		return *this;
	}

	packet& operator<<(int32_t value)
	{
		append<int32_t>(value);
		return *this;
	}

	packet& operator<<(int64_t value)
	{
		append<int64_t>(value);
		return *this;
	}

	// floating points
	packet& operator<<(float value)
	{
		append<float>(value);
		return *this;
	}

	packet& operator<<(double value)
	{
		append<double>(value);
		return *this;
	}

	packet& operator<<(const std::string& value)
	{
		append((uint8_t const*)value.c_str(), value.length());
		append((uint8_t)0);
		return *this;
	}

	packet& Write(const char* str, bool appendSize = true, bool addTerminateChar = false)
	{
		if (appendSize) 
			append((uint16_t)strlen(str));
		append((uint8_t const*)str, str ? strlen(str) : 0);
		if(addTerminateChar)
			append((uint8_t)0);
		return *this;
	}
	// Will always append string size before string with no terminator
	packet& operator<<(const char* str)
	{
		append((uint16_t)strlen(str));
		append((uint8_t const*)str, str ? strlen(str) : 0);
		//append((uint8_t)0);
		return *this;
	}

	packet& operator<<(bool const& val)
	{
		append((uint8_t const*)&val, 1);
		return *this;
	}

	void append(const std::string& str)
	{
		append((uint8_t const*)str.c_str(), str.size() + 1);
	}

	void append(const std::vector<uint8_t>& src)
	{
		return append(src.data(), src.size());
	}

	void append(const char* src, size_t cnt)
	{
		return append((const uint8_t*)src, cnt);
	}

	template<class T> void write_bytes(const T* src, size_t count, bool appendSize = false)
	{
		if (appendSize)
		{
			(*this) << (uint16_t)(count * sizeof(T));
		}
		append(src, count);
	}

	template<class T> void append(const T* src, size_t cnt)
	{
		return append((const uint8_t*)src, cnt * sizeof(T));
	}

	void append(const uint8_t* src, size_t cnt)
	{
		if (!cnt)
			return;

		assert(size() < 10000000);

		if (body.size() < _wpos + cnt)
			body.resize(_wpos + cnt);
		memcpy(&body[_wpos], src, cnt);
		_wpos += cnt;
	}

	template <std::size_t N>
	void WriteArray(std::array<uint8, N>& arr) {

		//append(&arr, N);
		if (body.size() < _wpos + N) {
			body.resize(_wpos + N);
		}
		
		std::memcpy(&body[_wpos], arr.data(), N);
		_wpos += N;
	}

	void append(const packet& buffer)
	{
		if (buffer.wpos())
			append(buffer.contents(), buffer.wpos());
	}

	// [EXTRACT FROM PACKET]
	packet& operator >> (bool& value)
	{
		value = read<bool>();
		return *this;
	}

	packet& operator >> (uint8_t& value)
	{
		value = read<uint8_t>();
		return *this;
	}

	packet& operator >> (uint16_t& value)
	{
		value = read<uint16_t>();
		return *this;
	}

	packet& operator >> (uint32_t& value)
	{
		value = read<uint32_t>();
		return *this;
	}

	packet& operator >> (uint64_t& value)
	{
		value = read<uint64_t>();
		return *this;
	}

	//signed as in 2e complement
	packet& operator >> (int8_t& value)
	{
		value = read<int8_t>();
		return *this;
	}

	packet& operator >> (int16_t& value)
	{
		value = read<int16_t>();
		return *this;
	}

	packet& operator >> (int32_t& value)
	{
		value = read<int32_t>();
		return *this;
	}

	packet& operator >> (int64_t& value)
	{
		value = read<int64_t>();
		return *this;
	}

	packet& operator >> (float& value)
	{
		value = read<float>();
		return *this;
	}

	packet& operator >> (double& value)
	{
		value = read<double>();
		return *this;
	}

	packet& operator >> (std::string& value)
	{
		value.clear();
		while (rpos() < size())                         // prevent crash at wrong string format in packet
		{
			char c = read<char>();
			if (c == 0)
				break;
			value += c;
		}
		return *this;
	}
	
	packet& read_bytes(std::string& value, size_t length)
	{
		value.clear();
		uint8_t i = 0;
		while (rpos() < size() && i < length)		 // prevent crash at wrong string format in packet
		{
			char c = read<char>();
			//if (c == 0)
			//	break;
			value += c;
			i++;
		}
		return *this;
	}

	// Looks at the next 2 bytes in the packet and reads that many chars
	packet& read_bytes(std::string& value)
	{
		value.clear();
		uint16_t length; (*this) >> length;
		uint16_t i = 0;
		while (rpos() < size() && i < length)		 // prevent crash at wrong string format in packet
		{
			char c = read<char>();

			// If we hit the terminate char we should not include it??
			if (c == 0)
			{
				_rpos++; // ?? DUNNO, probably
				break;
			}
				
			value += c;
			i++;
		}
		return *this;
	}

	//void read_bytes(std::vector<uint8_t>& vector, uint16_t size, bool append = false) {
	//
	//}

	// Since we cant know what the array max size is by only the pointer, we need that parameter also
	void read_bytes(uint8_t* dest, size_t arrayMaxSize) {
		uint16_t length; (*this) >> length;
		size_t allocSize = length + _rpos;
		//size_t arrayMaxSize = sizeof(*dest/sizeof(dest[0]));

		if (allocSize > body.size())
		{
			std::cerr << "Read buffer error, len: " << length << " size: " << body.size() << "\n";
			return;
		}
		if (length > arrayMaxSize)
		{
			std::cerr << "Read buffer error, destination too small: " << arrayMaxSize << " tried size: " << length << "\n";
			return;
		}
		std::cout << "Copied: " << length << "bytes" << std::endl;
		_rpos += length;
		memcpy(dest, &body[_rpos], length);
		//return *this;
	}

	//if (length > body.size())
	//{
	//	std::cerr << "Read buffer error, len: " << length << " size: " << body.size() << "\n";
	//	return;
	//}
	//size_t i = body.size() - length;
	//memcpy(dest, &body[0], length);
	//body.resize(i);

	/// <summary>
	/// Will read as many bytes as the size of the input array container and will throw an error if trying to read
	/// more than packet body contains and return without copying anything
	/// </summary>
	/// <typeparam name="N">Array size and how many bytes to copy into it</typeparam>
	/// <param name="arr">Input array to be modified</param>
	template <std::size_t N>
	void ReadToArray(std::array<uint8, N>& arr) {
		if (N + _rpos > size()) {
			spdlog::error("Tried to read past packet size!");
			return;
		}
		std::memcpy(arr.data(), &body[_rpos], N);
		_rpos += N;
	}

	void ReadPosition(glm::vec3& position) {
		std::array<uint8, 9> posData{};

		ReadToArray(posData);

		//spdlog::info("Pos read, X1:{}, X2:{}, X3:{} - Y1:{}, Y2:{}, Y3:{} - Z1:{}, Z2:{}, Z3:{}",
		//	posData[0], posData[1], posData[2], posData[3], posData[4], posData[5], posData[6], posData[7], posData[8]);

		position = archecore::utils::ConvertPosition(posData);
	}

	void WritePosition(float x, float y, float z) {
		WritePosition(glm::vec3(x, y, z));
	}

	void WritePosition(glm::vec3 position) {
		std::array<uint8, 9> pos = archecore::utils::ConvertPosition(position.x, position.y, position.z);

		//spdlog::info("Initial Pos, X1:{}, X2:{}, X3:{} - Y1:{}, Y2:{}, Y3:{} - Z1:{}, Z2:{}, Z3:{}",
		//	pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7], pos[8]);

		WriteArray(pos);
		//append(&pos, 9);

		//for (uint8 i = 0; i < 9; i++)
		//{
		//	(*this) << pos[i];
		//}
	}

	void WriteRotation(glm::i8vec3 sByteRotation) {
		(*this) << sByteRotation.x << sByteRotation.y << sByteRotation.z;
	}

	template <typename T> T read()
	{
		T r = read<T>(_rpos);
		_rpos += sizeof(T);
		return r;
	}

	template <typename T> T read(size_t pos) const
	{
		if (pos + sizeof(T) > size())
		{
			//throw packetException(false, pos, sizeof(T), size());
			std::cout << "Error packet read. Tried reading past packet end" << "\n";
			return NULL;
		}
		
		T val = *((T const*)&body[pos]);
		//EndianConvert(val);
		return val;
	}

	size_t rpos() const { return _rpos; }

	size_t rpos(size_t rpos_)
	{
		_rpos = rpos_;
		return _rpos;
	}

	size_t wpos() const { return _wpos; }

	size_t wpos(size_t wpos_)
	{
		_wpos = wpos_;
		return _wpos;
	}

private:
	// limited for internal use because can "append" any unexpected type (like pointer and etc) with hard detection problem
	template <typename T> void append(T value)
	{
		//EndianConvert(value);
		append((uint8_t*)&value, sizeof(T));
	}

protected:
	size_t _rpos = 0;	// rpos = position of the read head, wpos = position of the write head
	size_t _wpos = 2;
};

// Forward declare the session

//class WorldSession;
//
//struct world_packet
//{
//	std::shared_ptr<WorldSession> remote = nullptr;
//	packet msg;
//
//	// To easily make strings for debugging
//	friend std::ostream& operator << (std::ostream& os, const owned_packet& msg)
//	{
//		os << msg.msg;
//		return os;
//	}
//};

template <typename T>
inline packet& operator<<(packet& p, std::vector<T> const& v)
{
	//assert(v.size() < sizeof(uint16) * v.size());
	
	p << (uint16)v.size();

	for (auto const& elem : v) {
		p << elem;
	}

	return p;
}

template <typename T>
inline packet& operator>>(packet& p, std::vector<T>& v)
{
	uint16 vsize;
	p >> vsize;
	v.clear();
	while (vsize--)
	{
		T t;
		p >> t;
		v.push_back(t);
	}
	return p;
}

//class session;
template<typename T>
struct owned_packet
{
	std::shared_ptr<T> remote = nullptr;
	packet msg;

	// Again, a friendly string maker
	friend std::ostream& operator << (std::ostream& os, const owned_packet& msg)
	{
		os << msg.msg;
		return os;
	}
};

template<typename T>
constexpr T swap_endian(T value) {
	union {
		T value;
		std::uint8_t bytes[sizeof(T)];
	} u{ value };

	for (std::size_t i = 0; i < sizeof(T) / 2; ++i) {
		std::swap(u.bytes[i], u.bytes[sizeof(T) - i - 1]);
	}

	return u.value;
}