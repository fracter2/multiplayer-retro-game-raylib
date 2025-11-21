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
	
	

	void update(game& game_instance, const input::input_state& input_state) {
		

		if (game_instance.m_status == game::status::PRE_GAME) {
			// TODO Lobby? wait to recieve game start message

			return;
		}
		else if (game_instance.m_status == game::status::POST_GAME) {
			// TODO Win / lose screen / stats?

			return;
		}
		else if (game_instance.m_status == game::status::INVALID) {

			return;
		}

		// TODO Singleplayer mode?


		// ======== IN_GAME ========

		if (game_instance.m_state_queue.size() == 0) {			// Extrapolate
			// or... not? just freeze?
			// Extrapolate only to a degree?
			return;
		}
		else if (game_instance.m_state_queue[0].is_default()) { // Interpolate between current and next valid state
			
			int count = 0;
			while (game_instance.m_state_queue[count].is_default()) { count++; }

			std::vector<game_state> r = {};
			interp_game_states(game_instance.m_state, game_instance.m_state_queue[count], r, count);
			
		}
		else { // set state normally
			game_instance.m_prev_state = game_instance.m_state;
			game_instance.m_state = game_instance.m_state_queue[0];
			game_instance.m_state_queue.erase(game_instance.m_state_queue.begin());
		}

		game_instance.m_tick += 1;

		
		// Type safe const to reduce word lengths and to emphasise when it's mutable or not (to avoid setting accidently)
		const uint8			 user_index  = (uint8)game_instance.m_user_index;
		const uint32&		 tick		 = game_instance.m_tick;
		const game_state&	 state	     = game_instance.m_state;
		const player_entity& user_player = state.get_player(user_index);


		// INPUT PARSING
		player_entity::action current_action = {};
		const bool vertical_active = input_state.m_up != input_state.m_down;
		const bool horizontal_active = input_state.m_left != input_state.m_right;

		if (input_state.m_place_bomb &&	state.can_place_bomb(user_index, tick))
														   current_action = player_entity::action::PLACE_BOMB;
		else if (input_state.m_up    && vertical_active)   current_action = player_entity::action::MOVE_UP;
		else if (input_state.m_down  && vertical_active)   current_action = player_entity::action::MOVE_DOWN;
		else if (input_state.m_left  && horizontal_active) current_action = player_entity::action::MOVE_LEFT;
		else if (input_state.m_right && horizontal_active) current_action = player_entity::action::MOVE_RIGHT;
		else											   current_action = player_entity::action::STAND_STILL;

		game_instance.m_predict_actions.push_back(current_action);
		game_instance.m_actions_not_sent += 1;



		// Remove predicted actions that have been used by the server
		int ticks_ahead = game_instance.m_tick - user_player.m_prev_action_tick;
		int ticks_to_remove = (int)game_instance.m_predict_actions.size() - ticks_ahead;

		if (ticks_to_remove > 0) {
			game_instance.m_predict_actions.erase(game_instance.m_predict_actions.begin(), game_instance.m_predict_actions.begin() + ticks_to_remove);
			

		}

		
		// CLIENT SIDE PREDICTION
		game_state p_state = game_state(state);	// note: copied, not ref
		
		// TODO Apply predicted actions to client player and client bomb	// TODO MOVE GAME LOGIC OPERATIONS TO UTILITY CLASS TO SYNC BETWEEN CLIENT/SERVER
		
		game_instance.m_predicted_state = p_state;

	}


	

}