// server_game_system.hpp


#pragma once

#include "server_state.hpp"
#include "game.hpp"
#include "input.hpp"

namespace meteor::server_game_system {

	void update(game& game_instance, const input_state& input, server_state& server);

}