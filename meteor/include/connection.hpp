// connection.hpp

#pragma once

#include "network.hpp"

namespace meteor {
	

	struct connection {
		enum class status {
			DISCONNECTED,		// not connected, accepts broadcasts
			CONNECTING,			// attempting to connect to endpoint (sent connect pck, awaiting connect pck response with player index)
			CONNECTED,			// connected
			DISCONNECTING,		// attempting gracefull disconnect (sent disconnect pck awaiting response)
			TEXAS,				// texas
		};

		struct stats {
			static constexpr uint32 MAX_LOGGED_PACKETS = 20 * 10; // Max logged packets. 20/s sent packets * 10 sec
			
		};

		connection() = default;
		connection(ip_endpoint endpoint, status status = status::DISCONNECTED)
			: m_endpoint(endpoint)
			, m_last_recieve_time(0)
			, m_status(status)
			, m_send_sequence(0)
			, m_recieve_sequence(0)
			, m_recieve_acknowledge(0)
		{
		}

		//uint32		m_id				  = 0;
		ip_endpoint m_endpoint			  = {};
		status		m_status			  = status::DISCONNECTED;
		double		m_last_recieve_time   = 0;
		uint32		m_send_sequence		  = 0;
		uint32		m_recieve_sequence	  = 0;	// Used as send-ack
		uint32		m_recieve_acknowledge = 0;

		
	};

	


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