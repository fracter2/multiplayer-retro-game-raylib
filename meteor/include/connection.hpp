// connection.hpp

#pragma once

#include "network.hpp"

namespace meteor {
	struct connection {
		enum class status {
			DISCONNECTED,		// not connected, accept broadcasts
			CONNECTING,			// attempting to connect to endpoint (sent connect pck, awaiting connect pck response with player index)
			CONNECTED,			// connected
			DISCONNECTING,		// attempting gracefull disconnect (sent disconnect pck awaiting response)
			TEXAS,				// texas
		};

		connection() = default;
		connection(uint32 id, ip_endpoint endpoint, double last_recieve_time)
			: m_id(id)
			, m_endpoint(endpoint)
			, m_last_recieve_time(last_recieve_time)
			, m_status(status::DISCONNECTED)
			, m_sequence(0)
		{
		}

		uint32		m_id			 = 0;
		ip_endpoint m_endpoint;
		status		m_status		 = {};
		double		m_last_recieve_time = 0;
		uint32		m_sequence		 = 0; // What we send
		uint32		m_acknowledge	 = 0; // recieved sequence, used as ack
	};

	// Client -> server system state
	/*
	struct server_connection_syncer {
		server_connection_syncer() = default;

		connection m_connection		   = {};
		double	   m_last_checked_time = 0;
		//double	   m_next_update_time  = 0;
		//bool	   m_auto_connect	   = false;

	};
	*/
} // !meteor