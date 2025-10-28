// game.hpp

#pragma once

#include "raylib.h"
#include "raymath.h"
#include "network.hpp"
#include "messages.hpp"

//#define CLIENT

namespace meteor::game {

	// TODO Consider making a "game_config" class that is sent before game starts. Would need maps to be created/deleted each game (convinience thing)

	constexpr uint8  NAME_LENGTH_MAX		 = 16;
	constexpr double BOMB_EXPLOSION_TIME	 = 3.0;
	constexpr uint8	 BOMB_TIMER_TICKS		 = BOMB_EXPLOSION_TIME * TICK_RATE;
	constexpr double BOMB_PLACE_INTERVAL_MIN = 1.0;

	constexpr uint8  MAP_WIDTH				 = 16;
	constexpr uint8  MAP_HEIGHT				 = 16;

	constexpr int	 MAX_PLAYERS			 = 4;

	enum class tile_type : uint8 {
		INVALID,
		EMPTY,
		BREAKABLE,
		UNBREAKABLE
	};

	enum class gameplay_state : uint8 {
		INVALID,
		PRE_GAME,
		IN_GAME,
		POST_GAME
	};

	enum class player_state : uint8 {
		EMPTY,
		JOINING,
		ACTIVE,
		AFK,
		DISCONNECTED,
		TIMEOUT
	};

	// All player character actions, can only be performed one at a time per tick.
	// Should have a predictable way of being applied, used for latency state
	enum class player_actions : uint8 {
		STAND_STILL,
		MOVE_RIGHT,
		MOVE_LEFT,
		MOVE_UP,
		MOVE_DOWN,
		PLACE_BOMB
	};

	struct player_info {
		player_info() = default;

		//bool m_is_host	// noone is host since the server is running on a sepparate exe. 
							// The server exe could have admin tools, if needed.
		player_state m_player_state = player_state::EMPTY;
		char m_name[NAME_LENGTH_MAX] = "";
		// maybe lag info or similar could be here too
	};

	struct player_entity {
		player_entity() = default;
		player_entity(Vector2 position)
			: m_position(position)
		{
		}
		
		bool	m_dead = false;
		Vector2 m_position = {};
		Vector2 m_velocity = {};
	};

	struct bomb {
		bomb() = default;
		bomb(Vector2 position, int32 explosion_tick)
			: m_position(position)
			, m_explosion_tick(explosion_tick)
		{
		}
		Vector2 m_position = {};
		uint32	m_explosion_tick = 0;
	};

	struct tilemap {
		//tilemap() = default;

		tilemap() 
			: m_width(MAP_WIDTH)
			, m_height(MAP_HEIGHT)
		{
			m_tiles = new uint8[m_width * m_height];
		}
		/*	// We dont need a dynamic-size map now. If we do, we should consider the same for 
			   all other "game settings", and make a game_config class.
		tilemap(uint8 width, uint8 height)
			: m_width(width)
			, m_height(height)
		{
			m_tiles = new uint8[m_width * m_height];
		}
		*/

		const uint8 m_width;
		const uint8 m_height;
		uint8*		m_tiles;
		// Consider using bitmasks instead to reduce memory consumption
		//uint8* m_bits_breakable;
		//uint8* m_bits_unbreakable;

		const tile_type get_tile(const uint8 x, const uint8 y) const {
			if (x >= m_width || y >= m_height) return tile_type::INVALID;
			return (tile_type)(*(m_tiles + (x + y * m_width)));
		}

		void set_tile(const uint8 x, const uint8 y, const tile_type type) {
			assert(x < m_width && y < m_height);
			if (x >= m_width || y >= m_height) return;
			*(m_tiles + (x + y * m_width)) = (uint8)type;
		}
	};


	struct game_state {
		game_state() = default;
		
		player_entity m_players[MAX_PLAYERS] = {};
		bomb		  m_bombs[MAX_PLAYERS] = {};
		tilemap		  m_tilemap = {};

		const player_entity& get_player(const int index) const {
			assert(index < MAX_PLAYERS && index >= 0);
			if (index >= MAX_PLAYERS) return m_players[MAX_PLAYERS - 1];
			else if (index < 0)		  return m_players[0];
			else					  return m_players[index];
		}

		const bomb& get_bomb(const int index) const {
			assert(index < MAX_PLAYERS && index >= 0);
			if (index >= MAX_PLAYERS) return m_bombs[MAX_PLAYERS - 1];
			else if (index < 0)		  return m_bombs[0];
			else					  return m_bombs[index];
		}

		const tilemap& get_tilemap() const { return m_tilemap; }
		
	};

	struct game {
		game() = default;

		player_info m_player_info[MAX_PLAYERS] = {};

		game_state  m_current = {};
		uint32		m_current_tick = 0;

//#ifdef CLIENT					// rn im scared to use cause i dont understand
		int m_user_index = -1;	// index of local user client
		// queued (un-acked) actions 
		game_state  m_latency_state = {}; // m_current with un-acked input_actions
//#endif

	};


	// TODO SEPPARATE GAME_STATES INTO GAME_STATE AND LATENCY_STATES (with input history)
	// TODO SEPARATE UPDATE LOGIC FROM CONTAINER
	/*
	struct game {
		game() = default;

		// TODO Should be FIXED TIMESTEP delta at 60 or 64 ticks a second
		void update_process() {
			m_tick += 1;



			// TODO Update entities.
		}

		void update_entity(entity_state_message message) {

			int index = get_entity_index(message.m_id);
			if (index == -1) {
				add_entity(entity(message.m_id, message.m_position, message.m_color));
			}
			else {
				m_entities[index].m_position_prev = m_entities[index].m_position;
				m_entities[index].m_position = message.m_position;
				m_entities[index].m_color = message.m_color;
			}
		}

		// returns -1 on not found
		int get_entity_index(int32 id) const {
			for (int i = 0; i < m_entities.size(); i++) {
				if (m_entities[i].m_id == id) return i;
			}

			return -1;
		}

		void add_entity(entity entity) {

			m_entities.push_back(entity);
		}

		void render_frame() {
			ClearBackground(SKYBLUE);

			float lerp_fraction = (float)((GetTime() - m_time_sec) / NETWORK_TICK_SECONDS);
			lerp_fraction = Clamp(lerp_fraction, 0.0f, 1.0f);

			for (int i = 0; i < m_entities.size(); i++) {
				Vector2 pos = Vector2Lerp(
					m_entities[i].m_position_prev, 
					m_entities[i].m_position, 
					lerp_fraction);

				DrawRectangleV(pos, Vector2(10, 10), m_entities[i].m_color);
			}

			DrawFPS(2, 2);
		}

		std::vector<entity> m_entities = {};
		double m_time_sec = 0;
		int32 m_tick = 0;

		int32 m_player_id = -1;
	};
	*/
}
