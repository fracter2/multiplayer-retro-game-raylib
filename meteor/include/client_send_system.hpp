// client_send_system.hpp

#pragma once

#include "common.hpp"
#include "network.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "connection.hpp"
#include "game.hpp"

//#include "input.hpp"

namespace meteor::client_send_system {

	


	// Keep all dependencies as arguments to make them clearly visible
	void update(
		const uint32& ticks,
		udp_socket& socket,
		connection& connection,
		const ip_endpoint& client_endpoint,
		const game& game_instance
	);
	
	
	
}
