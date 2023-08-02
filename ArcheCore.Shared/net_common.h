#pragma once

#include "common_archecore.h"

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <map>
#include <unordered_map>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

namespace archecore::net 
{
	static uint32_t GetParsedIpAddress(const std::string& address)
	{
		try
		{
			asio::ip::address_v4 addressV4 = asio::ip::make_address_v4(address);
			return addressV4.to_uint();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

		return 0x00;
	}

	static uint32_t GetParsedIpAddressReverse(const char* address)
	{
		try
		{
			asio::ip::address_v4 addressV4 = asio::ip::make_address_v4(address);

			////asio::ip::address_v4::bytes_type byteAddress = addressV4.to_bytes();
			//asio::ip::address_v4::bytes_type byteAddress = addressV4.to_bytes();
			//std::reverse(byteAddress.begin(), byteAddress.end());
			//
			//addressV4 = asio::ip::make_address_v4(byteAddress);

			return addressV4.to_uint();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

		return 0x00;
	}
}


//static int db_callback(void* data, int argc, char** argv, char** azColName)
//{
//    int i;
//    fprintf(stderr, "%s: ", (const char*)data);
//
//    for (i = 0; i < argc; i++)
//    {
//        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//    }
//
//    printf("\n");
//    return 0;
//}