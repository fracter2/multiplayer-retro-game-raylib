// lag_optimizer.cpp

#pragma once

#include "lag_optimizer.hpp"

namespace meteor {

	constexpr int TARGET_QUEUED_STATES_ON_RECIEVE = 3;
	constexpr double TICK_SPEED_UP_AMOUNT = 0.002;	

	void lag_optimizer(double& next_tick_time, const game& game_instance) {

		if (game_instance.m_state_queue.size() > TARGET_QUEUED_STATES_ON_RECIEVE) {
			next_tick_time -= TICK_SPEED_UP_AMOUNT;
		}

	}

}