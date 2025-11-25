// connection.hpp

#pragma once

#include "network.hpp"
#include "common.hpp"
#include "protocol.hpp"
#include "raylib.h"

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

		static constexpr uint32 MAX_LOGGED_PACKETS = 20 * 5; // Max logged packets. 20/s * 5 sec

		connection();
		connection(ip_endpoint endpoint, status status = status::DISCONNECTED);

		bool can_recieve_payload(const payload_packet& packet) const noexcept;
		void log_recieve_payload(const payload_packet& packet, const uint32 size);
		void log_recieve_stream(const uint32 size);
		bool send_payload(udp_socket& socket, byte_stream& stream);
		bool send_stream(udp_socket& socket, byte_stream& stream);

		void set_disconnected();
		void set_disconnecting();
		void set_connected();
		void set_connecting();

		status get_status() const noexcept				{ return m_status; }
		double get_last_recieve_time() const noexcept	{ return m_recieve_times.front(); }
		uint32 get_send_sequence() const noexcept		{ return m_send_sequence; }
		uint32 get_recieve_sequence() const noexcept	{ return m_recieve_sequence; }
		uint32 get_recieve_acknowledge() const noexcept { return m_recieve_acknowledge; }

		const std::vector<uint32>& get_send_bytes_history() const noexcept		{ return m_send_bytes_history; }
		const std::vector<uint32>& get_recieve_bytes_history() const noexcept	{ return m_recieve_bytes_history; }
		const std::vector<double>& get_recieve_times() const noexcept			{ return m_recieve_times; }
		const std::vector<double>& get_rtt_history() const noexcept				{ return m_rtt_history; }

		double get_prev_rtt() const noexcept { if (m_rtt_history.empty()) return 0; else return m_rtt_history.front(); }


		ip_endpoint m_endpoint = {};
		bool m_debug_skip_recieve = false;


	private:
		status m_status				 = status::DISCONNECTED;
		//double m_last_recieve_time   = 0;
		uint32 m_send_sequence		 = 0;
		uint32 m_recieve_sequence	 = 0;
		uint32 m_recieve_acknowledge = 0;

		std::vector<uint32> m_send_bytes_history = { 0 };
		std::vector<uint32> m_recieve_bytes_history = { 0 };
		std::vector<double> m_recieve_times = { 0 };
		std::vector<double> m_rtt_history = { 0 };
		std::vector<double> m_un_acked_send_times = {};


		// Reliable messages history here? allow any message
		// std::vector<any message> m_reliable_messages_history		// maybe one std::vector for each message type?
		//std::vector<game_lobby_message> m_reliable_game_lobby_message = std::vector<game_lobby_message>();
		// Remove everything that has been ACKed
	};

} // !meteor