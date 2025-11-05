// render_system.hpp


#pragma once

#include "common.hpp"
#include "network.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "connection.hpp"
#include "game.hpp"

//#include "input.hpp"


namespace meteor::render_system {
	void render(const uint32& tick, const double time, const game::game& game_instance, const connection& conn, const Texture& m_texture);
}