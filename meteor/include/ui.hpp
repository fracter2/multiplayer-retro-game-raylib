// ui.hpp

#pragma once

#include "input.hpp"
#include "connection.hpp"

#ifdef _SERVER
#include "server_state.hpp"
#endif


namespace meteor::ui {

	void quit_check(connection& conn, const input_state& input);

	void debug_skip_recieve_check(connection& conn, const input_state& input);

	//void debug_send_recieve_check(connection& conn, const input_state& input);
#ifdef _SERVER
	void debug_skip_recieve_check(server_state& conn, const input_state& input);
#endif

}