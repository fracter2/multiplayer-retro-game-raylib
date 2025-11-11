// render.hpp

#pragma once

#include "common.hpp"
#include "game.hpp"
#include "connection.hpp"

#include "ui.hpp"




namespace meteor::render {

	namespace atlass {	// Texture atlass sources coordinates, manually aligned
		static constexpr Rectangle WALL = { 0, 0, 16, 16 };
		static constexpr Rectangle PLAYER = { 0, 0, 16, 16 };
		static constexpr Rectangle PLAYER_DEAD = { 0, 0, 16, 16 };
		static constexpr Rectangle BOMB = { 0, 0, 16, 16 };
	};

	static constexpr float BOMB_RADIUS = 12;
	static constexpr Color BOMB_COLOR = GRAY;
	static constexpr float PLAYER_RADIUS = 8;
	static constexpr Color PLAYER_COLOR = BLUE;
	static constexpr Color PLAYER_COLOR_DEAD = DARKBLUE;

	

	
	static void render_map(const Texture& texture, const tilemap& map) {
		for (int i = 0; i < tilemap::COUNT; i++) {
			if (!map.is_tile_active(i)) { continue; }

			Vector2i c = index_to_coord(i);
			const Vector2 pos = Vector2((float)c.x, (float)c.y) * (float)tilemap::TILE_PIXEL_LENGTH;
			const Rectangle destination{ pos.x, pos.y, (float)tilemap::TILE_PIXEL_LENGTH, (float)tilemap::TILE_PIXEL_LENGTH };
			DrawTexturePro(texture, atlass::WALL, destination, Vector2(0, 0), 0.0f, WHITE);
		}
	}

	static void render_bombs(const Texture& texture, const game& game_instance, const uint32& tick) {
		for (const bomb& da_bomb : game_instance.m_state.m_bombs) {
			if (da_bomb.m_explosion_tick >= tick) { DrawCircle((int)da_bomb.m_x, (int)da_bomb.m_y, BOMB_RADIUS, BOMB_COLOR); }
		}
	}

	static void render_characters(const Texture& texture, const game& game_instance) {
		for (const player_entity& player : game_instance.m_state.m_players) {
			if (player.m_dead) { DrawCircle((int)player.m_position.x, (int)player.m_position.x, PLAYER_RADIUS, PLAYER_COLOR_DEAD); }
			if (player.m_dead) { DrawCircle((int)player.m_position.x, (int)player.m_position.x, PLAYER_RADIUS, PLAYER_COLOR); }
		}
	}


	static void render_player_info(const Vector2i& coord, const game& game_instance) {
		
		constexpr int font_size = 18;
		constexpr Color color = MAROON;
		constexpr Color color_backdrop = BLACK;

		std::string str = "";

		int i = 0;
		for (player_info player : game_instance.m_player_info) {
			
			str += TextFormat("Player %d: %s ("
				, i
				, player.m_name);
			str += player_info::status_to_str(player.m_player_status);
			str += ")\n";
			
		}
		DrawText(str.c_str(), coord.x + 1, coord.y + 1, font_size, color_backdrop);
		DrawText(str.c_str(), coord.x, coord.y, font_size, color);


	}

}