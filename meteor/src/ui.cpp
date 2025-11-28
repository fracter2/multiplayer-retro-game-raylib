// menu.cpp

#pragma once

#include "ui.hpp"

namespace meteor::ui {


	void delay_tick(double& next_tick_time, input_state input) {
		if (input.m_7_just_pressed) {
			next_tick_time += 0.002;
		}
	}

	void quicken_tick(double& next_tick_time, input_state input) {
		if (input.m_8_just_pressed) {
			next_tick_time -= 0.002;
		}
	}

#ifdef _CLIENT
	void quit_check(connection& conn, const input_state& input) {
		if (input.m_1_just_pressed && conn.get_status() == connection::status::CONNECTED) {
			conn.set_disconnecting();
		}
	}


	void debug_skip_recieve_check(connection& conn, const input_state& input) {
		if (input.m_5) { conn.m_debug_skip_recieve = true; }
		else { conn.m_debug_skip_recieve = false; }
	}

	void discover_servers(const input_state& input, server_browser& browser, const game& state) {

		if (input.m_1_just_pressed && state.m_status == game::status::INVALID) {
			browser.m_entries.clear();
			browser.m_queue_discovery_send = true;
		}
	}

	void update_server_browser(const input_state& input, server_browser& browser, connection& conn) {
		if (input.m_up_just_pressed)   browser.m_select_index--;
		if (input.m_down_just_pressed) browser.m_select_index++;
		
		if (browser.m_select_index < 0)								 browser.m_select_index = 0;
		if (browser.m_select_index > (browser.m_entries.size() - 1)) browser.m_select_index = (int)(browser.m_entries.size() - 1);

		if (browser.m_entries.empty()) return;
		if (conn.get_status() != connection::status::DISCONNECTED) return;

		if (input.m_place_bomb_just_pressed) {
			conn = connection(
				browser.m_entries[browser.m_select_index].m_endpoint, 
				GetTime(), 
				connection::status::CONNECTING
			);

			debug::info("%g - selected server, attempting join", GetTime());
			debug::info("server endpoint: %d.%d.%d.%d:%d",
				conn.m_endpoint.m_address.a(),
				conn.m_endpoint.m_address.b(),
				conn.m_endpoint.m_address.c(),
				conn.m_endpoint.m_address.d(),
				conn.m_endpoint.port());
		}
	}

#endif

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