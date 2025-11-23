// game_update_system.hpp


#pragma once

#include "common.hpp"
#include "network.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "connection.hpp"
#include "game.hpp"

#include "input.hpp"
#include "input_to_player_action.hpp"


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



		// ======== IN_GAME ========

		if (game_instance.m_state_queue.size() == 0) {							// Extrapolate
			// or... not? just freeze?
			// Extrapolate only to a degree?
			return;
		}

		else if (game_instance.m_state_queue[0].is_default()) {					// Interpolate between current and next valid state
			int count = 0;
			while (game_instance.m_state_queue[count].is_default()) { count++; }

			std::vector<game_state> r = {};
			interp_game_states(game_instance.m_state, game_instance.m_state_queue[count], r);
			
			assert(count == r.size());

			count = 0;
			while (game_instance.m_state_queue[count].is_default()) { 
				game_instance.m_state_queue[count] = r[count];
				count++; 
			}
		}

		game_instance.m_prev_state = game_instance.m_state;
		game_instance.m_state = game_instance.m_state_queue[0];
		game_instance.m_state_queue.erase(game_instance.m_state_queue.begin());

		
		// Type safe const to reduce word lengths and to emphasise when it's mutable or not (to avoid setting accidently)
		const uint8			 user_index  = (uint8)game_instance.m_user_index;
		const uint32&		 tick		 = game_instance.m_state.m_tick;
		const game_state&	 state	     = game_instance.m_state;
		const player_entity& user_player = state.get_player(user_index);


		// INPUT PARSING
		game_instance.m_predict_actions.push_back(input_to_player_action(input_state, state, user_index));
		game_instance.m_actions_not_sent += 1;


		// Remove predicted actions that have been used by the server
		int ticks_ahead = game_instance.m_state.m_tick - user_player.m_prev_action_tick;
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