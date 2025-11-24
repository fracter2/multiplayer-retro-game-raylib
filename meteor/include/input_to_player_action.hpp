// input_to_player_action.hpp

#pragma once

#include "input.hpp"
#include "game.hpp"
#include "common.hpp"

namespace meteor {
	static player_entity::action input_to_player_action(const input_state& input, const game_state& state, const int player_index) {
		player_entity::action action = player_entity::action::INVALID;
		const bool vertical_active = input.m_up != input.m_down;
		const bool horizontal_active = input.m_left != input.m_right;

		if (input.m_place_bomb && state.can_place_bomb((uint8)player_index))
													 action = player_entity::action::PLACE_BOMB;
		else if (input.m_up && vertical_active)      action = player_entity::action::MOVE_UP;
		else if (input.m_down && vertical_active)    action = player_entity::action::MOVE_DOWN;
		else if (input.m_left && horizontal_active)  action = player_entity::action::MOVE_LEFT;
		else if (input.m_right && horizontal_active) action = player_entity::action::MOVE_RIGHT;
		else										 action = player_entity::action::STAND_STILL;

		return action;
	}
}