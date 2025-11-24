// menu.cpp

#pragma once

#include "ui.hpp"

namespace meteor::ui {
	void quit_check(connection& conn, const input_state& input) {
		if (input.m_1_just_pressed && conn.get_status() == connection::status::CONNECTED) {
			conn.set_disconnecting();
		}
	}

	void debug_skip_recieve_check(connection& conn, const input_state& input) {
		if (input.m_5) { conn.m_debug_skip_recieve = true; }
		else { conn.m_debug_skip_recieve = false; }
	}

#ifdef _SERVER

	void debug_skip_recieve_check(server_state& server, const input_state& input) {
		if (input.m_5) { server.m_debug_skip_recieve = true; }
		else { server.m_debug_skip_recieve = false; }
	}

	void debug_send_less_states(server_state& server, const input_state& input) {
		if (input.m_6_just_pressed) { 
			server.m_debug_send_less_states = !server.m_debug_send_less_states;
		}
	}

#endif
}