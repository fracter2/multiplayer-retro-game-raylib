// connection.cpp

#pragma once

#include "connection.hpp"

namespace meteor {
	connection::connection() {

		// Reserve space so it doesn't look like a memory leak
		m_recieve_bytes_history.reserve(MAX_LOGGED_PACKETS + 1);
		m_send_bytes_history.reserve(MAX_LOGGED_PACKETS + 1);
		m_rtt_history.reserve(MAX_LOGGED_PACKETS + 1);

	}

	connection::connection(ip_endpoint endpoint, status status)
		: m_endpoint(endpoint)
		, m_status(status)
		, m_send_sequence(0)
		, m_recieve_sequence(0)
		, m_recieve_acknowledge(0)
	{
		// Reserve space so it doesn't look like a memory leak
		m_recieve_bytes_history.reserve(MAX_LOGGED_PACKETS + 1);
		m_send_bytes_history.reserve(MAX_LOGGED_PACKETS + 1);
		m_rtt_history.reserve(MAX_LOGGED_PACKETS + 1);
	}

	bool connection::can_recieve_payload(const payload_packet& packet) const noexcept {
		if (m_status != connection::status::CONNECTED) {
			debug::info("%g - recieved payload package when irrelevant", GetTime());
			return false;
		}

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

	void connection::log_recieve_payload(const payload_packet& packet, const uint32 size) {
		log_recieve_stream(size);
		
		// Logg RTT
		int messages_just_acked = packet.m_acknowledge - m_recieve_acknowledge;
		assert(messages_just_acked >= 0); // dont read older packets!!

		assert(messages_just_acked <= m_un_acked_send_times.size());

		while (messages_just_acked > 0) {
			if (messages_just_acked > m_un_acked_send_times.size()) { 
				debug::error("%g - AJABAJA BROKEN SEQUENCING", GetTime()); 
				break;
			}

			m_rtt_history.insert(m_rtt_history.begin(), GetTime() - m_un_acked_send_times.front());
			m_un_acked_send_times.erase(m_un_acked_send_times.begin());
			messages_just_acked--;
		}
		while (m_rtt_history.size() >= MAX_LOGGED_PACKETS)
			m_rtt_history.pop_back();

		// Update state
		m_recieve_sequence = packet.m_sequence;
		m_recieve_acknowledge = packet.m_acknowledge;
	}

	void connection::log_recieve_stream(const uint32 size) {
		// Logg Bytes
		m_recieve_bytes_history.insert(m_recieve_bytes_history.begin(), size);
		while (m_recieve_bytes_history.size() >= MAX_LOGGED_PACKETS)
			m_recieve_bytes_history.pop_back();

		// Update state
		m_recieve_times.insert(m_recieve_times.begin(), GetTime());
		while (m_recieve_times.size() >= MAX_LOGGED_PACKETS)
			m_recieve_times.pop_back();
	}

	bool connection::send_payload(udp_socket& socket, byte_stream& stream) {
		if (!send_stream(socket, stream)) { return false; }

		m_send_sequence++;
		m_un_acked_send_times.push_back(GetTime());

		assert(m_un_acked_send_times.size() == m_send_sequence - m_recieve_acknowledge);
		if (!(m_un_acked_send_times.size() == m_send_sequence - m_recieve_acknowledge)) {
			debug::info("%g - AJABAJA BROKEN SEQUENCING", GetTime());
		}
		return true;
	}

	bool connection::send_stream(udp_socket& socket, byte_stream& stream) {
		// TODO fit reliable messages here, if there is space

		if (!socket.send_to(m_endpoint, stream)) { print_error_code(); return false; }

		m_send_bytes_history.insert(m_send_bytes_history.begin(), stream.size());
		while (m_send_bytes_history.size() >= MAX_LOGGED_PACKETS)
			m_send_bytes_history.pop_back();

		return true;
	}

	void connection::set_disconnected() {
		//conn.m_endpoint = {};				// We could let this stay until it is overriden by new player. This way we know who "recently deleted" 
											// and can reply with more disconnect packets, in case of packet loss.
		m_status = connection::status::DISCONNECTED;
		m_send_sequence = 0;
		m_recieve_sequence = 0;
		m_recieve_acknowledge = 0;

		m_send_bytes_history.clear();
		m_send_bytes_history = { 0 };

		m_recieve_bytes_history.clear();
		m_recieve_bytes_history = { 0 };

		m_rtt_history.clear();
		m_rtt_history = { 0 };

		m_un_acked_send_times.clear();
	}

	void connection::set_disconnecting() {
		m_status = connection::status::DISCONNECTING;
	}
	void connection::set_connected() {
		m_status = connection::status::CONNECTED;
	}
	void connection::set_connecting() {
		m_status = connection::status::CONNECTING;
	}
}



