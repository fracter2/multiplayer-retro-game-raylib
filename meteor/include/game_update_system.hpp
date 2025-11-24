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
	
	

	void update(game& game_instance, const input_state& input) {

		if (game_instance.m_status == game::status::PRE_GAME) {
			return;
		}
		else if (game_instance.m_status == game::status::POST_GAME) {
			return;
		}
		else if (game_instance.m_status == game::status::INVALID) {
			return;
		}


		else if (game_instance.m_status != game::status::IN_GAME) { 
			return; 
		}
		

		// ======== IN_GAME ========

		// Type safe const to reduce word lengths and to emphasise when it's mutable or not (to avoid setting accidently)
		const auto& state_queue = game_instance.m_state_queue;
		const auto& state = game_instance.m_state;

		if (state_queue.size() == 0) {
			// or... not? just freeze?
			// Extrapolate only to a degree?
			return;
		}

		// INTERPOLATE EMPTY STATES
		else if (state_queue[0].is_default()) {
			int count = 0;
			while (state_queue[count].is_default()) { count++; }

			std::vector<game_state> r = {};
			interp_game_states(state, state_queue[count], r);
			
			assert(count == r.size());

			count = 0;
			while (state_queue[count].is_default()) {
				game_instance.m_state_queue[count] = r[count];
				count++; 
			}
		}

		// SET NEXT STATE
		game_instance.m_prev_state = game_instance.m_state;
		game_instance.m_state = state_queue[0];
		game_instance.m_state_queue.erase(state_queue.begin());

		
		// INPUT PARSING
		const uint8	user_index  = (uint8)game_instance.m_user_index;
		auto& p_actions = game_instance.m_predict_actions;
		
		p_actions.push_back(input_to_player_action(input, state, user_index));
		game_instance.m_actions_not_sent += 1;


		// CLIENT SIDE PREDICTION
		// Remove predicted actions that have been used by the server
		int ticks_ahead = state.m_tick - state.m_players[user_index].m_prev_action_tick;
		int ticks_to_remove = (int)p_actions.size() - ticks_ahead;

		if (ticks_to_remove > 0) {
			p_actions.erase(p_actions.begin(), p_actions.begin() + (ticks_to_remove));	// second one isn't included!
		}

		game_state p_state = game_state(state);

		for (const player_entity::action action : p_actions) {
			p_state.m_tick += 1;
			p_state.m_players[user_index].m_prev_action = action;
			p_state.update_player(user_index);
		}

		game_instance.m_predicted_state = p_state;


		// DEBUG PRINT
		if (input.m_2_just_pressed) {
			debug::info("current tick: %d, action tick: %d, predict_actions.size(): %d, state_queue.size(): %d \n",
				state.m_tick,
				state.m_players[user_index].m_prev_action_tick,
				(uint32)p_actions.size(),
				(uint32)state_queue.size()
			);
		}
	}


	

}