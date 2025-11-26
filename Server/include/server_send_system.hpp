// server_send_system.hpp

#pragma once

#include "server_state.hpp"
#include "network.hpp"

namespace meteor::server_send_system {

	void update(
		const uint32& ticks,
		const double time,
		udp_socket& socket,
		server_state& server,
		const ip_endpoint& local_endpoint,
		const game& game_instance
	);
}

