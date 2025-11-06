// connection.hpp

#pragma once

#include "network.hpp"

namespace meteor {
	

	struct connection {
		enum class status {	// What each status means for their own behaviour on the client / server (has one for each client)
			DISCONNECTED,		// Client: Looking for game, accepts broadcasts
								// Server: This connection slot is available ( and possibly recently disconnected )
			CONNECTING,			// Client: Attempting to connect to server (sent connect pck, awaiting connect pck response with player index)
								// Server: Recieved connect package, sending connect pck back with his own player index, or disconnect to deny
			CONNECTED,			// Client: Connected, only sending payloads from now
								// Server: Connected, only sending payoads from now
			DISCONNECTING,		// Client: Attempting gracefull disconnect (sent disconnect pck awaiting response)
								// Server: Set to send a disconnect package, then set to disconnected. 
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
		double		m_last_recieve_time   = 0;
		status		m_status			  = status::DISCONNECTED;
		uint32		m_send_sequence		  = 0;
		uint32		m_recieve_sequence	  = 0;	// Used as send-ack
		uint32		m_recieve_acknowledge = 0;

		// Reliable messages history here? allow any message
		// std::vector<any message> m_reliable_messages_history		// maybe one std::vector for each message type?
		//std::vector<game_lobby_message> m_reliable_game_lobby_message = std::vector<game_lobby_message>();
		// Remove everything that has been ACKed
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