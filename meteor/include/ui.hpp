// Menu.hpp

#pragma once

#include "raylib.h"


namespace meteor::ui {

	struct main_menu {
		main_menu() = default;




		// Show available lobbies that have broadcast? Joins automatically for now...

		// TODO Username input field with text (allow input to check any keys just pressed and use here)
		// TODO Button to toggle joining broadcasts automatically? 

		// TODO input field for custom IP address and join button


		// TODO Display that shows iof you got kicked, timed out, disconnected disgracefully, or disconnected yourself
		// allow toggle on/off


		// TODO Display text that says you're waiting for a server

	};


	struct ui_box {
		ui_box(int x, int y, int width, int height)
			: m_x(x)
			, m_y(y)
			, m_width(width)
			, m_height(height)
		{
		}

		int m_x, m_y, m_width, m_height = 0;
	};

	struct button {
		button() = default;

		int x, y, width, height = 0;

	};


	struct ip_endpoint_input {
		ip_endpoint_input() = default;

	};

}