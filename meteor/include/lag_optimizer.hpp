// lag_optimizer.hpp

#pragma once

#include "input.hpp"
#include "game.hpp"

namespace meteor {

	void lag_optimizer(double& next_tick_time, const game& game_instance);

}