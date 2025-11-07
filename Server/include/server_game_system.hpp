// server_game_system.hpp


#pragma once

#include "server_state.hpp"
#include "game.hpp"
#include "input.hpp"

namespace meteor::server_game_system {

	void update(const uint32& tick, const double& dt, game& game_instance, const input::input_state& input_state, const server_state& server);

}