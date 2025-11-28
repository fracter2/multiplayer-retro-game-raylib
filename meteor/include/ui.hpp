// ui.hpp

#pragma once

#include "input.hpp"
#include "connection.hpp"

#ifdef _CLIENT
#include "server_browser.hpp"
#include "game.hpp"
#endif

#ifdef _SERVER
#include "server_state.hpp"
#endif


namespace meteor::ui {

	void delay_tick(double& next_tick_time, input_state input);
	void quicken_tick(double& next_tick_time, input_state input);

#ifdef _CLIENT
	void quit_check(connection& conn, const input_state& input);
	void debug_skip_recieve_check(connection& conn, const input_state& input);
	void discover_servers(const input_state& input, server_browser& browser, const game& state);
	void update_server_browser(const input_state& input, server_browser& browser, connection& conn);
#endif

#ifdef _SERVER
	void debug_skip_recieve_check(server_state& conn, const input_state& input);
	void debug_send_less_states(server_state& conn, const input_state& input);
#endif

}