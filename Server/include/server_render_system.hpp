// server_render_system.hpp


#pragma once

#include "render.hpp"
#include "server_state.hpp"

//#include "input.hpp"


namespace meteor::render {
	void server_system(const uint32& tick, const game& game_instance, const server_state& server, const Texture& texture);
}