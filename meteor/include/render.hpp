// render.hpp

#pragma once

#include "common.hpp"
#include "game.hpp"
#include "connection.hpp"

#include "raylib.h"
#include "ui.hpp"




namespace meteor::render {

	constexpr Vector2 ZERO = Vector2(0, 0);

	// TODO MOVE TO COMMON "render.hpp" OR SIMILAR with texture atlass + more
	namespace atlass {	// Texture atlass sources coordinates, manually aligned
		static constexpr Rectangle WALL = { 0, 0, 16, 16 };
		static constexpr Rectangle PLAYER = { 0, 0, 16, 16 };
		static constexpr Rectangle PLAYER_DEAD = { 0, 0, 16, 16 };
		static constexpr Rectangle BOMB = { 0, 0, 16, 16 };
	};

	

	
}