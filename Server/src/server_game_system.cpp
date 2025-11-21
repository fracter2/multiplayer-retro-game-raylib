// server_game_system.cpp


#pragma once

#include "server_game_system.hpp"

namespace meteor::server_game_system {


	void update(const uint32& tick, const double& dt, game& game_instance, const input::input_state& input_state, server_state& server) {


		if (input_state.m_2_just_pressed) {
			server.m_broadcast = !server.m_broadcast;
		}


		// ======== PRE_GAME ========
		if (game_instance.m_status == game::status::PRE_GAME) {
			if (input_state.m_3_just_pressed && game_instance.get_player_count() >= 2) {
				game_instance.queue_game_start = true;
				game_instance.init();
				server.m_status = server_state::status::ONLINE;
			}
			if (input_state.m_4_just_pressed && game_instance.get_player_count() < MAX_PLAYERS) {
				game_instance.fill_player_slots_with_bots();
				game_instance.game_lobby_changed = true;
			}

			return;
		}


		// ======== POST_GAME ========
		else if (game_instance.m_status == game::status::POST_GAME) {
			return;
		}

		// ======== INVALID ========
		else if (game_instance.m_status == game::status::INVALID) {
			if (input_state.m_1_just_pressed && server.m_status == server_state::status::OFFLINE) {
				game_instance.m_status = game::status::PRE_GAME;
				server.m_status = server_state::status::ONLINE_JOINABLE;

				// NOTE it does not reset the game or server
			}
			return;
		}

		// else...
		// ======== IN_GAME ========
		game_instance.push_state_to_history();

		// Apply player actions
		uint8 player_index = 0;
		for (player_entity& player : game_instance.m_state.m_players) {
			if (player.m_dead) continue;


			// Get next action, if there are any queued up
			if (!game_instance.m_player_action_queue[player_index].is_empty()) {
				std::pair<player_entity::action, uint32> r = game_instance.m_player_action_queue[player_index].read_next();
				player.m_prev_action = r.first;	
				player.m_prev_action_tick = r.second;
			}

			game_instance.m_state.update_player(player_index, tick);

			player_index++;
		}

		// Detonate any bomboes
		uint8 bomb_index = 0;
		for (const bomb& da_bomb : game_instance.m_state.m_bombs) {
			if (da_bomb.m_explosion_tick == game_instance.m_state.m_tick) {
				game_instance.m_state.apply_bomb_explosion(da_bomb);
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