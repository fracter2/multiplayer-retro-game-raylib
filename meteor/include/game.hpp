// game.hpp

#pragma once

#include "raylib.h"
//#include "raymath.h"
#include "common.hpp"
//#include "network.hpp"
//#include "messages.hpp"

//#define CLIENT

namespace meteor::game {

	// TODO Consider making a "game_config" class that is sent before game starts. Would need maps to be created/deleted each game (convinience thing)

	constexpr uint32 NAME_LENGTH_MAX		 = 16;
	constexpr double BOMB_FUSE_TIME			 = 3.0;
	constexpr uint32 BOMB_FUSE_TICKS		 = (uint32)(BOMB_FUSE_TIME * (double)TICK_RATE);
	constexpr uint32 BOMB_COOLDOWN_TICKS	 = (uint32)(1.0 * (double)TICK_RATE);

	constexpr uint8  MAP_WIDTH				 = 16;
	constexpr uint8  MAP_HEIGHT				 = 16;

	constexpr int	 MAX_PLAYERS			 = 4;
	constexpr int	 ACTIONS_BUFFER_LENGTH	 = 12;
	constexpr int    STATE_HISTORY_LENGTH	 = 30;

	// Right now map sizes are hard coded to simplify implementation (especially for network messages)
	constexpr int    TILEMAP_TILES			 = MAP_WIDTH * MAP_HEIGHT;
	constexpr int    TILEMAP_BYTES			 = 
		MAP_WIDTH * MAP_HEIGHT / 8 
		+ (((MAP_WIDTH * MAP_HEIGHT) % 8) == 0 ? 0 : 1);		// Add 1 if there's remainder, since "/" rounds down

	// Vec2 to tile coordniate system. Not clamped by tilemap size
	static void vec2_to_tile(const Vector2& pos, uint8& x, uint8& y) {
		uint8 new_x = (uint8)pos.x;
		uint8 new_y = (uint8)pos.y;

		x = new_x;
		y = new_y;
	}
	static bool valid_tile(const uint8 x, const uint8 y) {
		if (x >= MAP_WIDTH
			|| y >= MAP_HEIGHT
			|| (x + y * MAP_WIDTH) >= TILEMAP_TILES) return false;
		else return true;
	}


	// Player-user state, to keep track of game player slots.
	struct player_info {
		enum class status : uint8 {
			EMPTY,
			JOINING,
			ACTIVE,
			AFK,
			DISCONNECTED,
			TIMEOUT
		};

		player_info() = default;

		//bool m_is_host	// noone is host since the server is running on a sepparate exe. 
							// The server exe could have admin tools, if needed.
		status m_player_state = status::EMPTY;
		char   m_name[NAME_LENGTH_MAX] = "";
		// maybe lag info or similar could be here too
	};

	struct player_entity {
		// All player character actions, can only be performed one at a time per tick.
		// Should have a predictable way of being applied, used for latency state
		enum class action : uint8 {
			INVALID,		// Used to check validity of state
			STAND_STILL,
			MOVE_RIGHT,
			MOVE_LEFT,
			MOVE_UP,
			MOVE_DOWN,
			PLACE_BOMB
		};

		player_entity() = default;
		player_entity(Vector2 position);
		
		bool	m_dead = true;
		action	m_prev_action = action::INVALID;
		Vector2	m_position = {};
	};

	struct bomb {
		bomb() = default;
		bomb(uint8 x, uint8 y, int32 explosion_tick);
		uint8   m_x = 0;
		uint8   m_y = 0;
		uint32	m_explosion_tick = 0;
	};

	struct tilemap {
		tilemap() = default;
		
		uint8 m_tiles[TILEMAP_BYTES] = {};

		// Returns if tile is active (not broken)
		bool get_tile(const uint8 x, const uint8 y) const;

		void set_tile(const uint8 x, const uint8 y, bool value);
	};


	struct game_state {
		game_state() = default;

		player_entity m_players[MAX_PLAYERS] = {};
		bomb		  m_bombs[MAX_PLAYERS] = {};
		tilemap		  m_tilemap = {};

		const player_entity& get_player(const int index) const;
		const bomb& get_bomb(const int index) const;

		const tilemap& get_tilemap() const;
		
		const bool is_default() const;
	};


	struct game {
		enum class status : uint8 {
			INVALID,
			PRE_GAME,
			IN_GAME,
			POST_GAME
		};
		
		game() = default;

		player_info m_player_info[MAX_PLAYERS] = {};

		game_state  m_state = {};
		uint32		m_tick = 0;
		status		m_status = status::INVALID;

#ifdef _CLIENT
		int					  m_user_index = -1;								// index of local user client
		//player_entity::action m_predict_actions[ACTIONS_BUFFER_LENGTH] = {};	// un-acked actions by player, used to client-side-predict

		std::vector<player_entity::action> m_predict_actions = std::vector<player_entity::action>();

		game_state				m_predicted_state = {};							// result state from m_state having predicted actions applied.
		//game_state				m_state_queue[STATE_HISTORY_LENGTH] = {};
		std::vector<game_state> m_state_queue = std::vector<game_state>();
		//int						m_queued_states = 0;
#endif

#ifdef _SERVER
		// game_state or game_state_delta history for a couple ticks (half a sec worth?)
		//game_state m_state_history[STATE_HISTORY_LENGTH] = {};
		std::vector<game_state> m_state_history = std::vector<game_state>();
#endif



	};

	
}
