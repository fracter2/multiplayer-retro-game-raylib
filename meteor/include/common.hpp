// common.hpp


#pragma once

#include <cassert>
#include <vector>
#include <string_view>
#include <string>
#include <type_traits>

namespace meteor {
	using uint64 = unsigned long long;
	using  int64 = signed long long;
	using uint32 = unsigned int;
	using  int32 = signed int;
	using uint16 = unsigned short;
	using  int16 = signed short;
	using  uint8 = unsigned char;
	using   int8 = signed char;

	using yourmom = double long;
	using yourdad = long double;
	
	static constexpr int MAX_PLAYERS = 4;
	constexpr int TICK_RATE = 60;
	constexpr double TICK_TIME = 1.0 / TICK_RATE;
	constexpr uint32 TICKS_PER_NETWORK_SEND = 3;	// 60hz / 3 = 20hz
	constexpr int NETWORK_SEND_RATE = TICK_RATE / (int)TICKS_PER_NETWORK_SEND;

	constexpr uint16 PORT = 54321;

}