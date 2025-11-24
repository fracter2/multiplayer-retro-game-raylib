// server_render_system.cpp


#pragma once

#include "server_render_system.hpp"



namespace meteor::render {
	

	void server_system(const uint32& tick, const game& game_instance, const server_state& server, const Texture& texture) {
		// TODO CLEAR
		ClearBackground(SKYBLUE);


		{	// ==== ALWAYS ====
			render_map(texture, game_instance.m_state.get_tilemap());
			render_bombs(texture, game_instance.m_state);
			render_characters(texture, game_instance.m_state);
		}


		// ==== IN GAME ====
		if (game_instance.m_status == game::status::IN_GAME) {


			// TODO RENDER BACKGROUND


			// TODO RENDER NAMES (if there are names)

			// TODO REMDER INTERPOLATION FROM / TO (if toggled)

			// TODO REMDER CLIENT PREDICTION / SERVER_AUTH BOX for local player / bomb

		}


		// ==== PRE GAME ====
		else if (game_instance.m_status == game::status::PRE_GAME) {
			

			{ // Toggle broadcast and start game (key 2 & 3) Instructions
				constexpr int font_size = 20;
				constexpr Color color = MAROON;
				const char* text = TextFormat("Press '2' to toggle broadcasts (%s) and '3' to START GAME (%s) and '4' to add BOTS"
					, server.m_broadcast? "ON" : "OFF"
					, game_instance.get_player_count() >= 2? "CAN START      " : "TOO FEW PLAYERS"
				);
				const int text_width = MeasureText(text, font_size);
				const int text_x = (GetScreenWidth() - text_width) / 2;
				const int text_y = 8;
				DrawText(text, text_x + 1, text_y + 1, font_size, BLACK);
				DrawText(text, text_x, text_y, font_size, color);
				//DrawTextPro
			}
		}



		// ==== POST GAME ====
		else if (game_instance.m_status == game::status::POST_GAME) {
			// TODO RENDER WIN / LOSE

			//{ // Start game (Key 1) Instructions
			//	constexpr int font_size = 40;
			//	constexpr Color color = MAROON;
			//	const char* text = TextFormat("Press '1' to init server");
			//	const int text_width = MeasureText(text, font_size);
			//	const int text_x = (GetScreenWidth() - text_width) / 2;
			//	const int text_y = 8;
			//	DrawText(text, text_x + 1, text_y + 1, font_size, BLACK);
			//	DrawText(text, text_x, text_y, font_size, color);
			//}

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
				const int text_y = 8;
				DrawText(text.c_str(), text_x + 1, text_y + 1, font_size, BLACK);
				DrawText(text.c_str(), text_x, text_y, font_size, color);
			}

		}



		// ==== INVALID (MENU) ====
		else {

			

			{ // Start game (Key 1) Instructions
				constexpr int font_size = 40;
				constexpr Color color = MAROON;
				const char* text = TextFormat("Press '1' to init server");
				const int text_width = MeasureText(text, font_size);
				const int text_x = (GetScreenWidth() - text_width) / 2;
				const int text_y = 8;
				DrawText(text, text_x + 1, text_y + 1, font_size, BLACK);
				DrawText(text, text_x, text_y, font_size, color);
			}

		}



		// ==== STATISTICS ====
		{
			// RENDER FPS
			DrawFPS(2, GetScreenHeight() - 20);

			render_player_info(HUD_OFFSETI + Vector2i(8, 40), game_instance, server);

			float y_offset = 80;
			for (const connection& conn : server.m_clients) {
				render_rtt_graph(HUD_OFFSET + Vector2(300, y_offset), conn, -500.0f);			// seconds to ms, 1px per 2ms
				render_send_bytes_hist(HUD_OFFSET + Vector2(420, y_offset), conn, -0.25f);		// 1px per 4 bytes
				render_recieve_bytes_hist(HUD_OFFSET + Vector2(540, y_offset), conn, -0.25f);	// 1px per 4 bytes
				y_offset += 175;
			}
			


			/*Vector2i offset = HUD_OFFSET + Vector2(80, 20);
			for (const connection& conn : server.m_clients) {
				render_connection_stats(offset, game_instance, conn);
				offset = offset + Vector2i(0, 140);
			}*/
			// TODO RENDER RTT in ms (averaged across a second? sepparate peak?)

			// TODO RENDER BYTES SENT PER SECOND

			// TODO RENDER GRAPH WITH BYTES/s AND RTT
		}

	}

}