// render_system.cpp

#pragma once

#include "render.hpp"

namespace meteor::render{
	
	

	void client_system(const uint32& tick, const game& game_instance, const connection& conn, const Texture& texture) {

		// TODO CLEAR
		ClearBackground(SKYBLUE);

		// ==== IN GAME ====
		if (game_instance.m_status == game::status::IN_GAME) {

			render_map(texture, game_instance.m_predicted_state.get_tilemap());
			render_map_diff(texture, game_instance.m_state.get_tilemap(), game_instance.m_predicted_state.get_tilemap(), BLUE);
			
			render_bombs(texture, game_instance.m_state, BLUE);
			render_bombs(texture, game_instance.m_predicted_state);
			
			render_characters(texture, game_instance.m_state, BLUE);
			render_characters(texture, game_instance.m_predicted_state);

			render_interpolated_pos(texture, game_instance, DARKBLUE, MAROON);


			render_player_info(HUD_OFFSETI + Vector2i(8, 40), game_instance);
		}
		

		// ==== PRE GAME ====
		else if (game_instance.m_status == game::status::PRE_GAME) {

			render_player_info(HUD_OFFSETI + Vector2i(8, 40), game_instance);

			{ // "Waiting for server to start"
				constexpr int font_size = 40;
				constexpr Color color = MAROON;
				const char* text = TextFormat("Waiting for server to start");
				const int text_width = MeasureText(text, font_size);
				const int text_x = (GetScreenWidth() - text_width) / 2;
				const int text_y = 8;
				DrawText(text, text_x + 1, text_y + 1, font_size, BLACK);
				DrawText(text, text_x, text_y, font_size, color);
			}
		}
		


		// ==== POST GAME ====
		else if (game_instance.m_status == game::status::POST_GAME) {

			render_map(texture, game_instance.m_predicted_state.get_tilemap());
			render_bombs(texture, game_instance.m_predicted_state);
			render_characters(texture, game_instance.m_predicted_state);

			render_player_info(HUD_OFFSETI + Vector2i(8, 40), game_instance);

			{ // Display index of winner
				int winner_index = -1;
				int i = 0;
				for (const player_entity& player : game_instance.m_state.m_players) {
					if (!player.m_dead && winner_index == -1) {
						winner_index = i;
					}
					else if (!player.m_dead && winner_index >= 0) {
						winner_index = -2;
					}
					i++;
				}

				constexpr int font_size = 40;
				constexpr Color color = MAROON;
				std::string text = "";
				if (winner_index >= 0) {
					text += TextFormat("Player %d: %s Won!"
						, winner_index
						, game_instance.m_player_info[winner_index].m_name
					);
				}
				else {
					text += "No winner! Draw!";
				}

				const int text_width = MeasureText(text.c_str(), font_size);
				const int text_x = (GetScreenWidth() - text_width) / 2;
				const int text_y = 300;
				DrawText(text.c_str(), text_x + 1, text_y + 1, font_size, BLACK);
				DrawText(text.c_str(), text_x, text_y, font_size, color);
			}

		}
		


		// ==== INVALID / MENU ====
		else {
			{ // "In Menu"
				constexpr int font_size = 40;
				constexpr Color color = MAROON;
				const char* text = TextFormat("Not connected, waiting for server broadcast");
				const int text_width = MeasureText(text, font_size);
				const int text_x = (GetScreenWidth() - text_width) / 2;
				const int text_y = 8;
				DrawText(text, text_x + 1, text_y + 1, font_size, BLACK);
				DrawText(text, text_x, text_y, font_size, color);
			}

#ifdef _CLIENT
			if (game_instance.m_user_index != -1) { // "Disconnect reason"
				constexpr int font_size = 40;
				constexpr Color color = MAROON;
				std::string text = "Disconnect reason: ";
				text += player_info::status_to_str(game_instance.m_player_info[game_instance.m_user_index].m_player_status);
				const int text_width = MeasureText(text.c_str(), font_size);
				const int text_x = (GetScreenWidth() - text_width) / 2;
				const int text_y = 300;
				DrawText(text.c_str(), text_x + 1, text_y + 1, font_size, BLACK);
				DrawText(text.c_str(), text_x, text_y, font_size, color);
			}
#endif 

		}
		


		// ==== STATISTICS ====
		{
			// RENDER FPS
			DrawFPS(2, GetScreenHeight() - 20);
			
			int y_offset = 80 + 175 * (game_instance.m_user_index);	// Mimic server alignment for convenience

			render_rtt_graph(HUD_OFFSET + Vector2(300, (float)y_offset), conn, -500.0f);			// seconds to ms, 1px per 2ms
			render_send_bytes_hist(HUD_OFFSET + Vector2(420, (float)y_offset), conn, -0.25f);		// 1px per 4 bytes
			render_recieve_bytes_hist(HUD_OFFSET + Vector2(540, (float)y_offset), conn, -0.25f);	// 1px per 4 bytes

			render_connection_stats(Vector2i(80, 20), conn);
		}

	}
}