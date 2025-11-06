// render_system.cpp

#pragma once

//#include "render_system.hpp"
#include "render.hpp"
#include "raymath.h"

namespace meteor::render{
	

	void client_system(const uint32& tick, const double time, const game& game_instance, const connection& conn, const Texture& texture, ui::main_menu menu) {

		// TODO CLEAR
		ClearBackground(SKYBLUE);

		// ==== IN GAME ====
		if (game_instance.m_status == game::status::IN_GAME) {
			const tilemap& map = game_instance.m_state.get_tilemap();


			// TODO RENDER BACKGROUND

			// RENDER MAP
			for (int i = 0; i < tilemap::COUNT; i++) {
				if (!map.is_tile_active(i)) { continue; }

				uint8 x, y = 0;
				index_to_coord(i, x, y);

				const Vector2 position = tilemap::SIZE_V * Vector2(x, y);
				const Rectangle destination{ position.x, position.y, (float)tilemap::TILE_SIZE, (float)tilemap::TILE_SIZE };
				DrawTexturePro(texture, atlass::WALL, destination, ZERO, 0.0f, WHITE);
			}

			// TODO RENDER BOMBS

			// TODO RENDER CHARACTERS

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

			// TODO RENDER RTT in ms (averaged across a second? sepparate peak?)

			// TODO RENDER BYTES SENT PER SECOND

			// TODO RENDER GRAPH WITH BYTES/s AND RTT
		}

	}
}