// client_recieve_system.hpp

#pragma once

#include "common.hpp"
#include "network.hpp"
#include "connection.hpp"
#include "game.hpp"

namespace meteor::client_recieve_system {

	
	void update(
		double time,
		udp_socket& socket,
		connection& conn,
		game& game
	);
}