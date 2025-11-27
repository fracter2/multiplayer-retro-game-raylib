// server_recieve_system.hpp

#pragma once


#include "server_state.hpp"
#include "network.hpp"


namespace meteor::server_recieve_system {

	void update(
		server_state& server,
		udp_socket& socket,
		game& game,
		ip_endpoint& local_endpoint
	);

	void join_lobby(
		server_state& server, 
		game& game_instance, 
		const ip_endpoint& sender_endpoint, 
		const connect_packet& packet,
		const int stream_size
	);

}
