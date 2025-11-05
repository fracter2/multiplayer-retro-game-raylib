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
	
	bool can_place_bomb(const int index, const game::game_state& state, const uint32& tick) {
		const game::player_entity& user_player = state.m_players[index];
		uint8 x, y = 0;

		game::vec2_to_coord(user_player.m_position, x, y);

		bool r = true;
		r &= !user_player.m_dead;							// if not player dead...
		r &= game::valid_tile(x, y);						// and is inside map...
		r &= !state.m_tilemap.is_tile_active(x, y);				// and is not wall...

		for (const game::bomb& bomb : state.m_bombs) {		// and no other bombs are there...
			r &= !(bomb.m_x == x
				&& bomb.m_y == y
				&& bomb.m_explosion_tick > tick);
		}
		r &= (state.get_bomb(index).m_explosion_tick
			+ game::bomb::COOLDOWN_TICKS) < tick;			// and the bomb isn't already placed or in cooldown...

		return r;
	}

	void update(game::game game_instance, const input::input_state input_state) {
		

		if (game_instance.m_status == game::game::status::PRE_GAME) {
			// TODO Lobby? wait to recieve game start message

			return;
		}
		else if (game_instance.m_status == game::game::status::POST_GAME) {
			// TODO Win / lose screen / stats?

			return;
		}
		else if (game_instance.m_status == game::game::status::INVALID) {

			return;
		}

		// TODO Singleplayer mode?

		
		if (game_instance.m_state_queue.size() == 0) { // Extrapolate
			
		}
		else if (game_instance.m_state_queue[0].is_default()) { // Interpolate between current and next valid state

		}
		else { // set state normally
			game_instance.m_state = game_instance.m_state_queue[0];
			game_instance.m_state_queue.erase(game_instance.m_state_queue.begin());
		}

		game_instance.m_tick += 1;
		
		// Type safe const to reduce word lengths and to emphasise when it's mutable or not (to avoid setting accidently)
		const int				   user_index  = game_instance.m_user_index;
		const uint32			   tick		   = game_instance.m_tick;
		const game::game_state&	   state	   = game_instance.m_state;
		const game::player_entity& user_player = state.get_player(user_index);

		
		// Remove predicted actions that have been used by the server
		uint32 ticks_ahead  = game_instance.m_tick - user_player.m_prev_action_tick;
		int ticks_to_remove = (int)game_instance.m_predict_actions.size() - (int)ticks_ahead;

		game_instance.m_predict_actions.erase(game_instance.m_predict_actions.begin(), game_instance.m_predict_actions.begin() + ticks_to_remove); 
		//game_instance.m_last_sent_action_index -= ticks_to_remove;
		


		// INPUT PARSING
		game::player_entity::action current_action = {};
		const bool vertical_active = input_state.m_up != input_state.m_down;
		const bool horizontal_active = input_state.m_left != input_state.m_right;

		if (input_state.m_place_bomb && can_place_bomb(user_index, state, tick))
														   current_action = game::player_entity::action::PLACE_BOMB;
		else if (input_state.m_up    && vertical_active)   current_action = game::player_entity::action::MOVE_UP;
		else if (input_state.m_down  && vertical_active)   current_action = game::player_entity::action::MOVE_DOWN;
		else if (input_state.m_left  && horizontal_active) current_action = game::player_entity::action::MOVE_LEFT;
		else if (input_state.m_right && horizontal_active) current_action = game::player_entity::action::MOVE_RIGHT;
		else											   current_action = game::player_entity::action::STAND_STILL;

		game_instance.m_predict_actions.push_back(current_action);
		game_instance.m_actions_not_sent += 1;

		
		// CLIENT SIDE PREDICTION
		game::game_state p_state = game::game_state(state);	// note: copied, not ref
		
		// TODO Apply predicted actions to client player and client bomb	// TODO MOVE GAME LOGIC OPERATIONS TO UTILITY CLASS TO SYNC BETWEEN CLIENT/SERVER
		
		game_instance.m_predicted_state = p_state;

	}


	

}