// server_send_system.hpp

#pragma once

#include "server_state.hpp"
#include "network.hpp"

namespace meteor::server_send_system {
	constexpr uint32 TICKS_PER_UPDATE = 3;	// 60hz / 3 = 20hz


	void update(
		const uint32& ticks,
		const double time,
		udp_socket& socket,
		server_state& server,
		const ip_endpoint& local_endpoint,
		const game& game_instance
	);
}

