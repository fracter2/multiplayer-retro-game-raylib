// server_game_system.cpp


#pragma once

#include "server_game_system.hpp"
#include "server_state.hpp"

namespace meteor::server_game_system {


	void update(const uint32& tick, const double& dt, game& game_instance, const input::input_state& input_state, server_state& server) {


		if (input_state.m_1_just_pressed 
			&& server.m_status == server_state::status::OFFLINE) {		// Start server
			server.m_status = server_state::status::ONLINE_JOINABLE;
			game_instance.m_status = game::status::PRE_GAME;
		}
		if (input_state.m_2_just_pressed) {								// Toggle broadcasts
			server.m_broadcast = !server.m_broadcast;
		}
		if (input_state.m_3_just_pressed 
			&& game_instance.m_status == game::status::PRE_GAME 
			&& server.get_client_count() >= 2) {						// Start game
			game_instance.queue_game_start = true;
		}


		if (game_instance.m_status == game::status::PRE_GAME) {
			// TODO Lobby?

			return;
		}
		else if (game_instance.m_status == game::status::POST_GAME) {

			return;
		}
		else if (game_instance.m_status == game::status::INVALID) {

			return;
		}


		// ======== IN_GAME ========
		game_instance.m_tick += 1;
		game_instance.m_state_history.push_back(game_instance.m_state);
		game_instance.m_states_not_sent += 1;
		while (game_instance.m_state_history.size() >= game::STATE_HISTORY_LENGTH) {
			game_instance.m_state_history.erase(game_instance.m_state_history.begin());
		}

		// Apply player actions
		uint8 player_index = 0;
		for (player_entity player : game_instance.m_state.m_players) {
			if (player.m_dead) continue;


			// Get next action, if there are any queued up
			if (!game_instance.m_player_action_queue[player_index].is_empty()) {
				std::pair<player_entity::action, uint32> r = game_instance.m_player_action_queue[player_index].read_next();
				player.m_prev_action = r.first;				// This is set only here, not by m_state. Ideally we would communicate that better through wording/whatever
				player.m_prev_action_tick = r.second;
			}

			game_instance.m_state.apply_player_action(player_index, dt, tick);

			player_index++;
		}

		// Detonate any bomboes
		uint8 bomb_index = 0;
		for (const bomb& da_bomb : game_instance.m_state.m_bombs) {
			if (da_bomb.m_explosion_tick == game_instance.m_tick) {
				game_instance.m_state.apply_bomb_explosion(bomb_index);
			}
			bomb_index++;
		}

		// Check for winner
		uint8 players_alive = 0;
		uint8 alive_index = 0;		// To check for winner
		player_index = 0;
		for (const player_entity& player : game_instance.m_state.m_players) {
			if (!player.m_dead) {
				players_alive++; 
				alive_index = player_index;
			}
			player_index++;
		}

		if (players_alive <= 1) {
			
			game_instance.m_status = game::status::POST_GAME;

			for (int i = 0; i < MAX_PLAYERS; i++) {
				player_info& info = game_instance.m_player_info[i];
				if (info.m_player_status == player_info::status::ACTIVE) {
					info.m_player_status = player_info::status::LOSER;
				}
			}

			if (players_alive == 1) {
				game_instance.m_player_info[alive_index].m_player_status = player_info::status::WINNER;
			}

			game_instance.game_lobby_changed = true;
		}
		
	
	}

}