#pragma once

#include <cstdint>
#include <iostream>
#include <chrono>
#include <cstring>
#include <bitset>
#include <array>
#include <cmath>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

namespace archecore::utils
{
	constexpr double PI = 3.14159265;
	constexpr double PI_X2 = PI * 2;

	constexpr double ToShortDivider = ((double)1.0 / (double)32768.0);
	constexpr double ToSByteDivider = ((double)1.0 / (double)127.0);

	struct CompressedRotation {
		CompressedRotation(float roll, float pitch, float yaw) {
			ToRollPitchYawSBytes(roll, pitch, yaw);
		}

		void ToRollPitchYawSBytes(float x, float y, float z) {
			Roll = (int8)(x / PI_X2 / ToSByteDivider);
			Pitch = (int8)(y / PI_X2 / ToSByteDivider);
			Yaw = (int8)(z / PI_X2 / ToSByteDivider);
		}

		int8 Roll = 0;
		int8 Pitch = 0;
		int8 Yaw = 0;
	};


	static void PrintHex(uint16_t var)
	{
		printf("%04jx\n", (uintmax_t)var);
	}

	static void PrintHex(uint8_t var)
	{
		printf("%04jx\n", (uintmax_t)var);
	}

	template<typename T>
	void PrintHex(T var)
	{
		printf("%04jx\n", (uintmax_t)var);
	}

	// Check if a string contains any other characters than lower and upper-case letters (a-z, A-Z) and numbers (0-9)
	static bool ValidateCustomCharacters(std::string& string)
	{
		for (char& c : string)
		{
			if (c >= 48 && c <= 57 ||
				c >= 65 && c <= 90 ||
				c >= 97 && c <= 122)
			{
				continue;
			}
			else
			{
				std::cout << "String contained illegal letter: " << c << std::endl;
				PrintHex((uint8_t)c);
				return false;
			}
		}
		// Success
		return true;
	}

	static uint64_t GetUnixTimeNowInMilli()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	// Time since epoch in seconds
	static uint64_t GetUnixTimeNow()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	static int64_t GetUnixTimeNowSigned()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	static time_t GetTimeMin() {
		auto min_time = std::chrono::time_point<std::chrono::system_clock>::min();
		return std::chrono::system_clock::to_time_t(min_time);;
	}

	static void SwapBinary(std::uint32_t& value)
	{
		std::uint32_t tmp = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
		value = (tmp << 16) | (tmp >> 16);
	}

	static void ReverseString(std::string& str)
	{
		size_t len = str.length();

		for (uint8_t i = 0; i < len / 2; i++)
		{
			std::swap(str[i], str[len + i + 1]);
		}
	}

	static float DegToRad(float val) {
		return float((PI / 180) * val);
	}

	static float RadToDeg(float val) {
		return float(val / PI * 180);
	}

	static double DegToRad(double val) {
		return (PI / 180) * val;
	}

	static double RadToDeg(double val) {
		return val / PI * 180;
	}

	// Converts a string so first character is Uppercase and all the rest is lowerCase
	static void StringToNameFormat(std::string& str) {
		if (str.size() < 2) { std::cout << "ERROR name less than 2 characters..." << std::endl; return; }
		str[0] = std::toupper(str[0]);
		for (size_t i = 1; i < str.size(); i++)
		{
			str[i] = std::tolower(str[i]);
		}
	}

	static inline float ConvertLongX(int64 x) {
		return (float)((x >> 32) / (float)4096);
	}

	static inline int64 ConvertLongX(float x) {
		return (int64)(x * 4096) << 32;
	}

	static inline float ConvertLongY(int64 y)
	{
		return (float)((y >> 32) / (float)4096);
	}

	static inline int64 ConvertLongY(float y)
	{
		return (int64)(y * 4096) << 32;
	}

	double inline RoundToNEven(double num, int n) {
		double factor = std::pow(10.0, n);
		return glm::roundEven(num * factor) / factor;
	}

	static inline float ConvertSByteDirectionToDegree(int8 direction) {
		float angle = direction * (360.0f / 128);
		return fmod(angle, 360.0f);
	}

	static glm::vec3 ConvertPosition(std::array<uint8, 9> values) {
		glm::vec3 position;

		auto tempX = 8 * (values[0] + ((values[1] + (values[2] << 8)) << 8));
		auto flagX = (int32)(((-(values[8] & 0x80) >> 30) & 0xFFFFFFFE) + 1);
		auto resX = ((int64)tempX << 32) * flagX;

		auto tempY = 8 * (values[3] + ((values[4] + (values[5] << 8)) << 8));
		auto flagY = (int64)(((-(values[8] & 0x40) >> 30) & 0xFFFFFFFE) + 1);
		auto resY = ((int64)tempY << 32) * flagY;

		auto tempZ = (uint64)(values[6] + ((values[7] + ((values[8] & 0x3f) << 8)) << 8));

		position.x = ConvertLongX(resX);
		position.y = ConvertLongY(resY);
		position.z = (float)RoundToNEven(tempZ * 0.00000023841858 * 4196 - 100, 4);

		return position;
	}

	static std::array<uint8, 9> ConvertPosition(float x, float y, float z) {
		auto longX = ConvertLongX(x);
		auto longY = ConvertLongX(y);

		auto preX = longX >> 31;
		auto preY = longY >> 31;

		auto resultX = (preX ^ (longX + preX + (0 > preX ? 1 : 0))) >> 3;
		auto resultY = (preY ^ (longY + preY + (0 > preY ? 1 : 0))) >> 3;
		auto resultZ = (int64)floor((z + 100.0) / 4196.0 * 4194304.0 + 0.5);

		std::array<uint8, 9> position{};
		position[0] = (uint8)(resultX >> 32);
		position[1] = (uint8)(resultX >> 40);
		position[2] = (uint8)(resultX >> 48);

		position[3] = (uint8)(resultY >> 32);
		position[4] = (uint8)(resultY >> 40);
		position[5] = (uint8)(resultY >> 48);

		position[6] = (uint8)resultZ;
		position[7] = (uint8)(resultZ >> 8);
		position[8] = (uint8)(((resultZ >> 16) & 0x3F) + (((y < 0 ? 1 : 0) + static_cast<uint64>(2) * (x < 0 ? 1 : 0)) << 6));

		//spdlog::info("X1:{}, X2:{}, X3:{} - Y1:{}, Y2:{}, Y3:{} - Z1:{}, Z2:{}, Z3:{}", 
		//	(uint16)position[0], (uint16)position[1], (uint16)position[2], 
		//	(uint16)position[3], (uint16)position[4], (uint16)position[5], 
		//	(uint16)position[6], (uint16)position[7], (uint16)position[8]);

		return position;
	}

	/// <summary>
	/// Get least significant bytes to the specified size
	/// </summary>
	static void getLSB(uint32 value, uint8 numBytes, uint8* buffer) {
		for (uint8 i = 0; i < numBytes; i++) {
			buffer[i] = static_cast<uint8>(value & 0xFF);
			value >>= 8;
		}
	}
}