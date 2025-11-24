// menu.cpp

#pragma once

#include "ui.hpp"


namespace meteor::ui {
	void quit_check(connection& conn, const input::input_state& input) {
		if (input.m_2_just_pressed && conn.get_status() == connection::status::CONNECTED) {
			conn.set_disconnecting();
		}
	}
}