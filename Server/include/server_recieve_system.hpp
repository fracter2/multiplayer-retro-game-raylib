// server_recieve_system.hpp

#pragma once


#include "server_state.hpp"
#include "network.hpp"


namespace meteor::server_recieve_system {

	void update(
		const double time, 
		server_state& server,
		udp_socket& socket,
		game& game,
		ip_endpoint& local_endpoint
	);

	void join_lobby(
		const double& time,
		server_state& server, 
		udp_socket& socket, 
		game& game_instance, 
		byte_stream_reader& reader,
		ip_endpoint sender_endpoint,
		connect_packet packet);

}
