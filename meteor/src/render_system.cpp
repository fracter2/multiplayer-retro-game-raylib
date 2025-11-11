// render_system.cpp

#pragma once

//#include "render_system.hpp"
#include "render.hpp"

namespace meteor::render{
	
	

	void client_system(const uint32& tick, const double time, const game& game_instance, const connection& conn, const Texture& texture, ui::main_menu menu) {

		// TODO CLEAR
		ClearBackground(SKYBLUE);

		// ==== IN GAME ====
		if (game_instance.m_status == game::status::IN_GAME) {
			const tilemap& map = game_instance.m_state.get_tilemap();


			// TODO RENDER BACKGROUND

			// RENDER MAP
			render_map(texture, map);

			// TODO RENDER BOMBS
			render_bombs(texture, game_instance, tick);

			// TODO RENDER CHARACTERS
			render_characters(texture, game_instance);

			// TODO RENDER NAMES (if there are names)

			// TODO REMDER INTERPOLATION FROM / TO (if toggled)

			// TODO REMDER CLIENT PREDICTION / SERVER_AUTH BOX for local player / bomb

		}
		

		// ==== PRE GAME ====
		else if (game_instance.m_status == game::status::PRE_GAME) {
			// TODO RENDER GAME LOBBY (if in lobby

		}
		


		// ==== POST GAME ====
		else if (game_instance.m_status == game::status::POST_GAME) {
			// TODO RENDER WIN / LOSE

		}
		


		// ==== INVALID / MENU ====
		else {
			// TODO RENDER MENU

			// TODO RENDER BUTTONS / INPUT AREAS

			// TODO RENDER AVAILABLE SERVERS IN NETWORK

			// TODO RENDER POPUPS / NOTIFICATIONS (timeout or disconnect)
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