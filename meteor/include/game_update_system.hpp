// game_update_system.hpp


#pragma once

#include "common.hpp"
#include "network.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "connection.hpp"
#include "game.hpp"

#include "input.hpp"


namespace meteor::game_update_system {

	constexpr int MAX_PACKET_SIZE = 1024;

	void update(game::game game, const input::input_state) {

	}
}