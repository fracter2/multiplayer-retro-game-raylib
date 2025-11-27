// client_recieve_system.hpp

#pragma once

#include "common.hpp"
#include "network.hpp"
#include "connection.hpp"
#include "game.hpp"
#include "server_browser.hpp"

namespace meteor::client_recieve_system {

	
	void update(
		double& next_tick_time,
		udp_socket& socket,
		connection& conn,
		game& game,
		server_browser& browser
	);
}