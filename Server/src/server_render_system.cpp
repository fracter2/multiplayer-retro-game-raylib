// server_render_system.cpp


#pragma once

#include "server_render_system.hpp"



namespace meteor::render {
	

	void server_system(const uint32& tick, const double time, const game& game_instance, const server_state& server, const Texture& texture, ui::main_menu& menu) {
		// TODO CLEAR
		ClearBackground(SKYBLUE);

		// ==== IN GAME ====
		if (game_instance.m_status == game::status::IN_GAME) {
			const tilemap& map = game_instance.m_state.get_tilemap();


			// TODO RENDER BACKGROUND

			
			render_map(texture, map);
			render_bombs(texture, game_instance, tick);
			render_characters(texture, game_instance);

			// TODO RENDER NAMES (if there are names)

			// TODO REMDER INTERPOLATION FROM / TO (if toggled)

			// TODO REMDER CLIENT PREDICTION / SERVER_AUTH BOX for local player / bomb

		}


		// ==== PRE GAME ====
		else if (game_instance.m_status == game::status::PRE_GAME) {
			


			{ // Toggle broadcast and start game (key 2/3) Instructions
				constexpr int font_size = 20;
				constexpr Color color = MAROON;
				const char* text = TextFormat("Press '2' to toggle broadcasts (%s) and '3' to START GAME (%s)"
					, server.m_broadcast? "BROADCASTING" : "OFF         "
					, server.get_client_count() >= 2? "CAN START      " : "TOO FEW PLAYERS"
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

		}



		// ==== INVALID (MENU) ====
		else {
			// TODO RENDER BUTTONS / INPUT AREAS

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

			const Vector2i coord = Vector2i(8, 40);
			render_player_info(coord, game_instance);

			// TODO RENDER RTT in ms (averaged across a second? sepparate peak?)

			// TODO RENDER BYTES SENT PER SECOND

			// TODO RENDER GRAPH WITH BYTES/s AND RTT
		}

	}

}