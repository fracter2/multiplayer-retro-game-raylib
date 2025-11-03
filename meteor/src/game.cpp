// game.cpp
#pragma once

#include "game.hpp"


namespace meteor::game {

	void vec2_to_tile(const Vector2& pos, uint8& x, uint8& y) {
		uint8 new_x = (uint8)pos.x;
		uint8 new_y = (uint8)pos.y;

		x = new_x;
		y = new_y;
	}


}