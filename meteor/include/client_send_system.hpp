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

	constexpr uint32 TICKS_PER_UPDATE = 3;	// 60hz / 3 = 20hz
	//constexpr double CONNECTING_RESEND_DELAY_TICKS = 5; // 5 network-ticks

	
	
	struct system {
		system() = default;

		// Should only contain metadata or data needed between updates. 

		//double m_next_update_time = 0;
		uint32 m_update_counter = 0;			// Counts normal ticks (when update runs) to check when to actually do a network send
		uint32 m_total_network_ticks = 0;		// Just out of curiosity
		//uint32 m_total_packages_sent
		//uint32 m_total_bytes_sent



		
	};

	// Keep all dependencies as arguments to make them clearly visible
	void update(
		const uint32& ticks,
		udp_socket& socket,
		connection& connection,
		const ip_endpoint& client_endpoint,
		const game& game_instance
	);
	
	
	
}
