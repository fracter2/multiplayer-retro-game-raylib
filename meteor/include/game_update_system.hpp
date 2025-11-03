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

		game::vec2_to_tile(user_player.m_position, x, y);

		bool r = true;
		r &= !user_player.m_dead;							// if not player dead...
		r &= game::valid_tile(x, y);						// and is inside map...
		r &= !state.m_tilemap.get_tile(x, y);				// and is not wall...

		for (const game::bomb& bomb : state.m_bombs) {		// and no other bombs are there...
			r &= !(bomb.m_x == x
				&& bomb.m_y == y
				&& bomb.m_explosion_tick > tick);
		}
		r &= (state.get_bomb(index).m_explosion_tick
			+ game::BOMB_COOLDOWN_TICKS) < tick;			// and the bomb isn't already placed or in cooldown...

		return r;
	}

	void update(game::game game, const input::input_state input_state) {
		
		// TODO Increment tick
		


		/*
		if (game.m_state_queue[0].is_default()) {
			if (game.m_state_queue == 0) {	
				// TODO Extrapolate from prev
			}
			else {
				// TODO Interpolate state
			}
			
		}
		*/

		// INPUT PARSING
		const game::player_entity& user_player = game.m_state.m_players[game.m_user_index];
		game::player_entity::action current_action = {};
		bool vertical_active = input_state.m_up != input_state.m_down;
		bool horizontal_active = input_state.m_left != input_state.m_right;

		if (input_state.m_place_bomb && can_place_bomb(game.m_user_index, game.m_state, game.m_tick)) 
														   current_action = game::player_entity::action::PLACE_BOMB;
		else if (input_state.m_up    && vertical_active)   current_action = game::player_entity::action::MOVE_UP;
		else if (input_state.m_down  && vertical_active)   current_action = game::player_entity::action::MOVE_DOWN;
		else if (input_state.m_left  && horizontal_active) current_action = game::player_entity::action::MOVE_LEFT;
		else if (input_state.m_right && horizontal_active) current_action = game::player_entity::action::MOVE_RIGHT;
		else											   current_action = game::player_entity::action::STAND_STILL;

		game.m_predict_actions.push_back(current_action);


		// CLIENT SIDE PREDICTION
		game::game_state p_state = game::game_state(game.m_state);	// note: copied, not ref
		
		// TODO Apply predicted actions to client player and client bomb
		
		game.m_predicted_state = p_state;

	}


	

}