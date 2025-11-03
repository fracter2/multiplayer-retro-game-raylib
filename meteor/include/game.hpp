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
	constexpr int	 ACTIONS_BUFFER_LENGTH	 = 12;
	constexpr int    STATE_HISTORY_LENGTH	 = 30;

	constexpr int    TILEMAP_TILES			 = MAP_WIDTH * MAP_HEIGHT;
	constexpr int    TILEMAP_BYTES			 = 
		MAP_WIDTH * MAP_HEIGHT / 8 
		+ (((MAP_WIDTH * MAP_HEIGHT) % 8) == 0 ? 0 : 1);		// Add 1 if there's remainder, since "/" rounds down

	// If we use a uint8, this is how it could be divided.
	// Alternatively, we use a bitmask for breakable / unbreakable tiles, to save space.
	/*
	enum class tile_type : uint8 {
		INVALID,
		EMPTY,
		BREAKABLE,
		UNBREAKABLE
	};
	*/

	enum class gameplay_state : uint8 {
		INVALID,
		PRE_GAME,
		IN_GAME,
		POST_GAME
	};

	// Player-user state, to keep track of game player slots.
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
		INVALID,
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
		
		bool		   m_dead = true;
		player_actions m_prev_action = {};
		Vector2		   m_position = {};
	};

	struct bomb {
		bomb() = default;
		bomb(uint8 x, uint8 y, int32 explosion_tick)
			: m_x(x)
			, m_y(y)
			, m_explosion_tick(explosion_tick)
		{
		}
		uint8   m_x = 0;
		uint8   m_y = 0;
		uint32	m_explosion_tick = 0;
	};

	struct tilemap {
		tilemap() = default;

		// No dynamic map for now, to maintain simplicity
		/*
		tilemap() 
			//: m_width(MAP_WIDTH)
			//, m_height(MAP_HEIGHT)
		{
			//m_tiles = new uint8[total_bytes()];
		}


		//const uint8 m_width;
		//const uint8 m_height;
		

		const int total_tiles() const { return MAP_WIDTH * MAP_HEIGHT; }
		
		const int total_bytes() const {
			return
				MAP_WIDTH * MAP_HEIGHT / 8
				+ ((MAP_WIDTH * MAP_HEIGHT) % 8) == 0 ? 0 : 1;		// Add 1 if there's remainder, since "/" rounds down
		}

		//uint8* m_tiles;
		*/
		
		uint8 m_tiles[TILEMAP_BYTES] = {};

		const bool valid_tile(const uint8 x, const uint8 y) const {
			if (x >= MAP_WIDTH
			 || y >= MAP_HEIGHT
			 || (x + y * MAP_WIDTH) >= TILEMAP_TILES) return false;
			else return true;
		}

		const bool get_tile(const uint8 x, const uint8 y) const {
			assert(valid_tile(x, y));
			uint8 byte	  = *(m_tiles + ((x + y * MAP_WIDTH) / 8));
			uint8 bitmask = (uint8)1 << ((x + y * MAP_WIDTH) % 8);

			return (byte & bitmask) != 0;	// & is the bitwise "and" operator, so if the result is higher than 0, that bit is active.
		}

		void set_tile(const uint8 x, const uint8 y, bool value) {
			assert(valid_tile(x, y));
			uint8 byte = *(m_tiles + ((x + y * MAP_WIDTH) / 8));
			uint8 bitmask = (uint8)1 << ((x + y * MAP_WIDTH) % 8);

			if (value) { byte = byte | bitmask; }			// | is bitwise "or", resulting in all 1s being kept from both
			else	   { byte = byte & (~bitmask); }		// ~ is bitwise complement operator, flipping all bits 1->0 and 0->1
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
		
		const bool is_default() const {
			return m_players[0].m_prev_action == player_actions::INVALID;
		}
	};

	struct game {
		game() = default;

		player_info m_player_info[MAX_PLAYERS] = {};

		game_state  m_state = {};
		uint32		m_tick = 0;

#ifdef _CLIENT
		int m_user_index = -1;	// index of local user client
		player_actions m_predict_actions[ACTIONS_BUFFER_LENGTH] = {};	// un-acked actions by player, used to client-side-predict
		game_state	   m_predicted_state = {};							// result state from m_state having predicted actions applied.
		game_state	   m_state_queue[STATE_HISTORY_LENGTH] = {};
		int			   m_queued_states = 0;
#endif

#ifdef _SERVER
		// game_state or game_state_delta history for a couple ticks (half a sec worth?)
		game_state m_state_history[STATE_HISTORY_LENGTH] = {};
#endif



	};

	// ---- OLD CODE for mouse-syncing tests ----

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
