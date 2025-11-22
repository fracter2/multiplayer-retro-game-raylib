// connection.cpp

#pragma once

#include "connection.hpp"

namespace meteor {
	connection::connection(ip_endpoint endpoint, status status)
		: m_endpoint(endpoint)
		, m_last_recieve_time(0)
		, m_status(status)
		, m_send_sequence(0)
		, m_recieve_sequence(0)
		, m_recieve_acknowledge(0)
	{
	}

	bool connection::can_recieve(const payload_packet& packet) const {
		if (m_status != connection::status::CONNECTED) {
			debug::info("%g - recieved payload package when irrelevant", GetTime());
			return false;
		}

		// Sequence
		if (packet.m_sequence <= m_recieve_sequence) {
			debug::info("out-of-order or duplicate packet dropped. my recieve_sequenece: %d, packet sequence: %d, time: %f "
				, (m_recieve_sequence)
				, (packet.m_sequence)
				, (GetTime())
			);
			return false;
		}
		return true;
	}

	void connection::log_payload(const payload_packet& packet, const uint32 size) {
		// Logg RTT
		int messages_just_acked = packet.m_acknowledge - m_recieve_acknowledge;
		assert(messages_just_acked >= 0); // dont read older packets!!

		while (messages_just_acked > 0) {
			m_rtt_history.insert(m_rtt_history.begin(), GetTime() - m_un_acked_send_times.front());
			m_un_acked_send_times.pop_back();
		}
		while (m_rtt_history.size() >= MAX_LOGGED_PACKETS)
			m_rtt_history.pop_back();

		// Logg Bytes
		m_recieve_bytes_history.insert(m_recieve_bytes_history.begin(), size);
		while (m_recieve_bytes_history.size() >= MAX_LOGGED_PACKETS)
			m_recieve_bytes_history.pop_back();

		// Update
		m_last_recieve_time = GetTime();
		m_recieve_sequence = packet.m_sequence;
		m_recieve_acknowledge = packet.m_acknowledge;
	}

	bool connection::send(udp_socket& socket, byte_stream& stream) {
		// TODO fit reliable messages here, if there is space

		if (!socket.send_to(m_endpoint, stream)) { print_error_code(); return false; }

		// Logg send time and bytes
		m_un_acked_send_times.push_back(GetTime());

		m_send_bytes_history.insert(m_send_bytes_history.begin(), stream.size());
		while (m_send_bytes_history.size() >= MAX_LOGGED_PACKETS)
			m_send_bytes_history.pop_back();

		m_send_sequence++;

		return true;
	}

	void connection::set_disconnected() {
		//conn.m_endpoint = {};				// We could let this stay until it is overriden by new player. This way we know who "recently deleted" 
										// and can reply with more disconnect packets, in case of packet loss.
		m_status = connection::status::DISCONNECTED;
		m_send_sequence = 0;
		m_recieve_sequence = 0;
		m_recieve_acknowledge = 0;
	}
}



