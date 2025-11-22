// render.hpp

#pragma once

#include "common.hpp"
#include "game.hpp"
#include "connection.hpp"

#include "ui.hpp"




namespace meteor::render {

	namespace atlass {	// Texture atlass sources coordinates, manually aligned
		static constexpr Rectangle BACKGROUND = { 0, 0, 16, 16 };
		static constexpr Rectangle WALL = { 16, 0, 16, 16 };
		static constexpr Rectangle PLAYER = { 32, 0, 16, 16 };
		static constexpr Rectangle PLAYER_DEAD = { 48, 0, 16, 16 };
		static constexpr Rectangle BOMB = { 64, 0, 16, 16 };
	};

	static constexpr int WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 720;

	static constexpr float BOMB_RADIUS = 12;
	static constexpr Color BOMB_COLOR = GRAY;
	static constexpr float PLAYER_RADIUS = 8;
	static constexpr Color PLAYER_COLOR = BLUE;
	static constexpr Color PLAYER_COLOR_DEAD = DARKBLUE;
	static constexpr Color PLAYER_NUM_COLOR = BLACK;
	static constexpr int   PLAYER_NUM_SIZE = 6;

	static constexpr Vector2 MAP_SIZE = Vector2((float)tilemap::WIDTH * (float)tilemap::TILE_PIXEL_LENGTH, (float)tilemap::HEIGHT * (float)tilemap::TILE_PIXEL_LENGTH);
	//static constexpr Vector2 MAP_OFFSET = Vector2(((WINDOW_WIDTH - MAP_SIZE.x) / 2), 80);
	static constexpr Vector2 MAP_OFFSET = Vector2(80, 80);
	static constexpr Vector2 HUD_OFFSET = Vector2(MAP_OFFSET.x + MAP_SIZE.x, MAP_OFFSET.y);

	

	
	static void render_map(const Texture& texture, const tilemap& map) {
		for (int i = 0; i < tilemap::COUNT; i++) {
			Vector2i c = index_to_coord(i);
			const Vector2 pos = MAP_OFFSET + Vector2((float)c.x, (float)c.y) * (float)tilemap::TILE_PIXEL_LENGTH;
			const Rectangle destination{ pos.x, pos.y, (float)tilemap::TILE_PIXEL_LENGTH, (float)tilemap::TILE_PIXEL_LENGTH };
			DrawTexturePro(
				texture, 
				(map.is_tile_active(i) ? atlass::WALL : atlass::BACKGROUND),
				destination, 
				Vector2(0, 0), 
				0.0f, 
				WHITE);
		}
	}

	static void render_bombs(const Texture& texture, const game& game_instance) {
		for (const bomb& da_bomb : game_instance.m_state.m_bombs) {
			if (da_bomb.m_explosion_tick >= game_instance.m_state.m_tick) {
				const Vector2 pos = MAP_OFFSET + Vector2((float)da_bomb.m_x, (float)da_bomb.m_y) * (float)tilemap::TILE_PIXEL_LENGTH;
				const Rectangle destination{ pos.x, pos.y, (float)tilemap::TILE_PIXEL_LENGTH, (float)tilemap::TILE_PIXEL_LENGTH };
				DrawTexturePro(
					texture,
					atlass::BOMB,
					destination,
					Vector2(0, 0),
					0.0f,
					WHITE);
			}
		}
	}

	static void render_characters(const Texture& texture, const game& game_instance) {
		int i = 0;
		for (const player_entity& player : game_instance.m_state.m_players) {
			const Vector2 pos = MAP_OFFSET + player.m_position * (float)tilemap::TILE_PIXEL_LENGTH;
			const Rectangle destination{ pos.x, pos.y, (float)tilemap::TILE_PIXEL_LENGTH, (float)tilemap::TILE_PIXEL_LENGTH };
			DrawTexturePro(
				texture,
				player.m_dead? atlass::PLAYER_DEAD : atlass::PLAYER,
				destination,
				tilemap::TILE_PIXEL_CENTER_OFFSET,
				0.0f,
				WHITE);
			//DrawTextPro(GetFontDefault(), TextFormat("%d", i), player.m_position, Vector2Zero(),0, PLAYER_NUM_SIZE, 0, PLAYER_NUM_COLOR);
			i++;
		}
	}


	static void render_player_info(const Vector2i& coord, const game& game_instance) {
		
		constexpr int font_size = 18;
		constexpr Color color = MAROON;
		constexpr Color color_backdrop = BLACK;

		std::string str = "";

		int i = 0;
		for (player_info player : game_instance.m_player_info) {
			
			const double input_delay = ((double)(game_instance.m_state.m_tick) - (double)(game_instance.m_state.m_players[i].m_prev_action_tick)) * TICK_TIME;

			str += TextFormat("Player %d: %s \n   input_delay: %f \n   status: ("
				, i
				, player.m_name
				, input_delay
			);
			str += player_info::status_to_str(player.m_player_status);
			str += ")\n\n";
			i++;
		}
		const int x = (int)HUD_OFFSET.x + coord.x;
		const int y = (int)HUD_OFFSET.y + coord.y;
		DrawText(str.c_str(), x + 1, y + 1, font_size, color_backdrop);
		DrawText(str.c_str(), x, y, font_size, color);


	}

	static void render_connection_stats(const Vector2i& coord, const game& game_instance, const connection& conn) {
		constexpr int font_size = 12;
		constexpr Color color = MAROON;
		constexpr Color color_backdrop = BLACK;

		std::string str = "";

		// TODO Write connection RTT (+ history graph?)
		

		str += TextFormat("input_delay: %f "
			, conn.get_prev_rtt()
		);

		const int x = (int)coord.x;
		const int y = (int)coord.y;
		DrawText(str.c_str(), x + 1, y + 1, font_size, color_backdrop);
		DrawText(str.c_str(), x, y, font_size, color);
	}

}