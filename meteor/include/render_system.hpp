// render_system.hpp


#pragma once

#include "render.hpp"


//#include "input.hpp"


namespace meteor::render {
	void client_system(const uint32& tick, const game& game_instance, const connection& conn, const Texture& texture, const server_browser& browser);
}