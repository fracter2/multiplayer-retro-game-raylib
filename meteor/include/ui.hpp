// ui.hpp

#pragma once

#include "input.hpp"
#include "connection.hpp"


namespace meteor::ui {

	void quit_check(connection& conn, const input::input_state& input);

}