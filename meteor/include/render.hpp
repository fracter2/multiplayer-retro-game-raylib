// render.hpp

#pragma once

#include "common.hpp"
#include "game.hpp"
#include "connection.hpp"

#ifdef _SERVER
#include "server_state.hpp"
#endif // _SERVER



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
	static constexpr Vector2i HUD_OFFSETI = Vector2i((int)MAP_OFFSET.x + (int)MAP_SIZE.x, (int)MAP_OFFSET.y);

	


	static void render_tile(const Texture& texture, const tilemap& map, const int i, const Color tint = WHITE) {
		Vector2i c = index_to_coord(i);
		const Vector2 pos = MAP_OFFSET + Vector2((float)c.x, (float)c.y) * (float)tilemap::TILE_PIXEL_LENGTH;
		const Rectangle destination{ pos.x, pos.y, (float)tilemap::TILE_PIXEL_LENGTH, (float)tilemap::TILE_PIXEL_LENGTH };
		DrawTexturePro(
			texture,
			(map.is_tile_active(i) ? atlass::WALL : atlass::BACKGROUND),
			destination,
			Vector2(0, 0),
			0.0f,
			tint);
	}

	
	static void render_map(const Texture& texture, const tilemap& map, const Color tint = WHITE) {
		for (int i = 0; i < tilemap::COUNT; i++) {
			render_tile(texture, map, i, tint);
		}
	}

	// Renders tiles on the first map if it is different from the diff_map
	static void render_map_diff(const Texture& texture, const tilemap& map, const tilemap& other_map, const Color tint = WHITE) {
		for (int i = 0; i < tilemap::COUNT; i++) {
			if (map.is_tile_active(i) != other_map.is_tile_active(i)) {
				render_tile(texture, map, i, tint);
			}
		}
	}

	static void render_bombs(const Texture& texture, const game_state& state, const Color tint = WHITE) {
		for (const bomb& da_bomb : state.m_bombs) {
			if (da_bomb.m_explosion_tick >= state.m_tick) {
				const Vector2 pos = MAP_OFFSET + Vector2((float)da_bomb.m_x, (float)da_bomb.m_y) * (float)tilemap::TILE_PIXEL_LENGTH;
				const Rectangle destination{ pos.x, pos.y, (float)tilemap::TILE_PIXEL_LENGTH, (float)tilemap::TILE_PIXEL_LENGTH };
				DrawTexturePro(
					texture,
					atlass::BOMB,
					destination,
					Vector2(0, 0),
					0.0f,
					tint);
			}
		}
	}

	static void render_characters(const Texture& texture, const game_state& state, const Color tint = WHITE) {
		int i = 0;
		for (const player_entity& player : state.m_players) {
			const Vector2 pos = MAP_OFFSET + player.m_position * (float)tilemap::TILE_PIXEL_LENGTH;
			const Rectangle destination{ pos.x, pos.y, (float)tilemap::TILE_PIXEL_LENGTH, (float)tilemap::TILE_PIXEL_LENGTH };
			DrawTexturePro(
				texture,
				player.m_dead? atlass::PLAYER_DEAD : atlass::PLAYER,
				destination,
				tilemap::TILE_PIXEL_CENTER_OFFSET,
				0.0f,
				tint);
			//DrawTextPro(GetFontDefault(), TextFormat("%d", i), player.m_position, Vector2Zero(),0, PLAYER_NUM_SIZE, 0, PLAYER_NUM_COLOR);
			i++;
		}
	}

#ifdef _CLIENT
	static void render_interpolated_pos(const Texture& texture, const game& game_instance, const Color tint_from = WHITE, const Color tint_to = WHITE) {
		if (!game_instance.m_state.m_interpolated) return;
		
		game_state next_non_interp_state;		
		for (const game_state& state : game_instance.m_state_queue) {	// Guaranteed to find a non-interpolated state because you can only interpolate between 2 valid states.
			if (!state.m_interpolated) {
				next_non_interp_state = game_state(state);
				break;
			}
		}

		const int offset = - (int)tilemap::TILE_PIXEL_LENGTH / 2 + (int)MAP_OFFSET.x;
		const int length = (int)tilemap::TILE_PIXEL_LENGTH;
		for (const player_entity& player : game_instance.m_prev_non_interp_state.m_players) {
			const Vector2 pos = Vector2(player.m_position * (float)tilemap::TILE_PIXEL_LENGTH);
			DrawRectangleLines((int)pos.x + offset, (int)pos.y - offset, length, length, tint_from);
		}
		
		for (const player_entity& player : next_non_interp_state.m_players) {
			const Vector2 pos = Vector2(player.m_position * (float)tilemap::TILE_PIXEL_LENGTH);
			DrawRectangleLines((int)pos.x + offset, (int)pos.y - offset, length, length, tint_to);
		}
		
	}
#endif // _CLIENT

	// Returns sec as ms with one digit after '.'
	static std::string sec_to_ms_str_pretty(const double sec) {
		std::string r = "";
		r += std::to_string(sec * 1000);
		if (r.find('.') + 2 < r.size()) {
			r.erase(r.begin() + r.find('.') + 2, r.end());
		}
		r += "ms";
		return r;
	}

	static void render_connection_stats(const Vector2i& coord, const connection& conn) {
		constexpr int font_size = 12;
		constexpr Color color = MAROON;
		constexpr Color color_backdrop = BLACK;

		std::string str = "";

		// TODO Write connection RTT (+ history graph?)


		str += "RTT: " + sec_to_ms_str_pretty(conn.get_prev_rtt());

		const int x = (int)coord.x;
		const int y = (int)coord.y;
		DrawText(str.c_str(), x + 1, y + 1, font_size, color_backdrop);
		DrawText(str.c_str(), x, y, font_size, color);
	}

	static void render_player_info(const Vector2i& coord, const game& game_instance) {
		
		constexpr int font_size = 18;
		constexpr Color color = MAROON;
		constexpr Color color_backdrop = BLACK;

		std::string str = "";

		int i = 0;
		for (player_info player : game_instance.m_player_info) {
			
			const double input_delay = ((double)(game_instance.m_state.m_tick) - (double)(game_instance.m_state.m_players[i].m_prev_action_tick)) * TICK_TIME;

			str += TextFormat("Player %d: %s \n   input delay: "
				, i
				, player.m_name
			);
			str += sec_to_ms_str_pretty(input_delay);
			str += "\n   status: (";
			str += player_info::status_to_str(player.m_player_status);
			str += ")\n\n";
			i++;
		}
		const int x = coord.x;
		const int y = coord.y;
		DrawText(str.c_str(), x + 1, y + 1, font_size, color_backdrop);
		DrawText(str.c_str(), x, y, font_size, color);
	}

#ifdef _SERVER
	static void render_player_info(const Vector2i& coord, const game& game_instance, const server_state& server) {

		constexpr int font_size = 18;
		constexpr Color color = DARKGRAY;
		constexpr Color color_backdrop = BLACK;

		std::string str = "";

		int i = 0;
		for (player_info player : game_instance.m_player_info) {

			const double input_delay = ((double)(game_instance.m_state.m_tick) - (double)(game_instance.m_state.m_players[i].m_prev_action_tick)) * TICK_TIME;

			str += TextFormat("Player %d: %s \n   input delay: "
				, i
				, player.m_name 
			);
			str += sec_to_ms_str_pretty(input_delay);
			str += "\n   RTT: ";
			str += sec_to_ms_str_pretty(server.m_clients[i].get_prev_rtt());
			str += "\n   status: (";
			str += player_info::status_to_str(player.m_player_status);
			str += ")\n\n";
			i++;
		}
		const int x = coord.x;
		const int y = coord.y;
		DrawText(str.c_str(), x + 1, y + 1, font_size, color_backdrop);
		DrawText(str.c_str(), x, y, font_size, color);
	}
#endif

	static void render_rtt_graph(const Vector2 offset, const connection& conn, const float range_mult) {
		const auto& hist = conn.get_rtt_history();
		for (int i = 0; i < hist.size(); i++) {
			DrawLineV(offset + Vector2((float)i, 0), offset + Vector2((float)i, (float)hist[i] * range_mult), DARKBLUE);
		}
	}

	static void render_send_bytes_hist(const Vector2 offset, const connection& conn, const float range_mult) {
		const auto& hist = conn.get_send_bytes_history();
		for (int i = 0; i < hist.size(); i++) {
			DrawLineV(offset + Vector2((float)i, 0), offset + Vector2((float)i, (float)hist[i] * range_mult), DARKGREEN);
		}
	}

	static void render_recieve_bytes_hist(const Vector2 offset, const connection& conn, const float range_mult) {
		const auto& hist = conn.get_recieve_bytes_history();
		for (int i = 0; i < hist.size(); i++) {
			DrawLineV(offset + Vector2((float)i, 0), offset + Vector2((float)i, (float)hist[i] * range_mult), DARKPURPLE);
		}
	}

}