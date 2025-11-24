// game.hpp

#pragma once

#include <raylib.h>
#include <raymath.h>
#include "common.hpp"


//#include "network.hpp"
//#include "messages.hpp"

//#define CLIENT

namespace meteor {

	static constexpr int MAX_PLAYERS = 4;
	constexpr int TICK_RATE = 60;
	constexpr double TICK_TIME = 1.0 / TICK_RATE;


	/*
	struct tile {
		tile() = default;
		tile(const uint8& x, const uint8& y);

		uint8 x, y = 0;
		tile operator+(const tile& a) { tile t = tile(x + a.x, y + a.y); assert(t.x > x && t.y > y); return tile(x + a.x, y + a.y); }
		tile operator-(const tile& a) { return tile(x - a.x, y - a.y); }
		tile operator*(const tile& a) { return tile(x * a.x, y * a.y); }
		tile operator/(const tile& a) { return tile(x / a.x, y / a.y); }


		template<std::integral<> T> Vector2i operator*(T a) { return Vector2i(x * a, y * a); }
		//template<std::integral<> T> Vector2i operator/(Vector2i a) { return Vector2i(x / a.x, y / a.y); }	// Is this even needed? 
	};*/

	struct Vector2i {
		constexpr Vector2i() = default;
		constexpr Vector2i(const int& x, const int& y) : x(x), y(y) {};
		constexpr Vector2i(const Vector2& a) : x((int)a.x), y((int)a.y) {};

		int x = 0;
		int y = 0;
		Vector2i operator+(const Vector2i& a) const { return Vector2i(x + a.x, y + a.y); }
		Vector2i operator-(const Vector2i& a) const { return Vector2i(x - a.x, y - a.y); }
		Vector2i operator*(const Vector2i& a) const { return Vector2i(x * a.x, y * a.y); }
		Vector2i operator/(const Vector2i& a) const { return Vector2i(x / a.x, y / a.y); }

		bool operator==(const Vector2i a) const { return x == a.x && y == a.y; }

		template<std::integral<> T> Vector2i operator*(const T& a) const { return Vector2i((int)(x * a), (int)(y * a)); }
		//template<std::integral<> T> Vector2i operator/(Vector2i a) { return Vector2i(x / a.x, y / a.y); }	// Is this even needed? 
	};

#define Vector2i_ZERO Vector2i(0, 0)
#define Vector2i_UP Vector2i(0, -1)
#define Vector2i_DOWN Vector2i(0, 1)
#define Vector2i_LEFT Vector2i(-1, 0)
#define Vector2i_RIGHT Vector2i(1, 0)

	struct tilemap {
		static constexpr uint32 TILE_PIXEL_LENGTH = 32;		// Pixels, for rendering // TODO RENAME OR MOVE
		static constexpr Vector2 TILE_PIXEL_CENTER_OFFSET = Vector2(TILE_PIXEL_LENGTH/2, TILE_PIXEL_LENGTH/2);		// Pixels, for rendering // TODO RENAME OR MOVE
		static constexpr uint8  WIDTH = 16;
		static constexpr uint8  HEIGHT = 16;
		//static constexpr Vector2 SIZE_V = Vector2(WIDTH, HEIGHT);

		// Right now map sizes are hard coded to simplify implementation (especially for network messages)
		static constexpr int COUNT = WIDTH * HEIGHT;
		static constexpr int BYTES_NEEDED =
			WIDTH * HEIGHT / 8
			+ (((WIDTH * HEIGHT) % 8) == 0 ? 0 : 1);		// Add 1 if there's remainder, since "/" rounds down

		//struct coord;
		//struct index;

		tilemap() = default;

		uint8 m_tiles[BYTES_NEEDED] = {};

		// Returns if tile is active (aka not destroyed)
		bool is_tile_active(const Vector2i& coord) const;
		bool is_tile_active(const int& index) const;

		void set_tile(const Vector2i& coord, const bool& value);
		void set_tile(const int& index, const bool& value);
		void set_all(const bool& value);

		//		TODO Consider finishing these as safe structs for thir values. coord would have to validate after each x/y (or force const)
		/*
		struct index {
			index() = default;
			template<std::integral<> T> index(const T& a);
			index(const coord& a);

			const uint32 v = 0;
		};

		struct coord {
			coord() = default;
			template<std::integral<> T> coord(const T& ax, const T& ay);
			coord(const uint8& ax, const uint8& ay);
			coord(const Vector2i& a);
			coord(const index& a);

			coord operator+(const coord& a) { return coord(x + a.x, y + a.y); }
			coord operator-(const Vector2i& a) { return coord(x - a.x, y - a.y); }
			coord operator*(const Vector2i& a) { return coord(x * a.x, y * a.y); }
			coord operator/(const Vector2i& a) { return coord(x / a.x, y / a.y); }

			const uint8 x, y = 0;
		};
		*/
	};

	// if it's inside the map boundries
	static bool is_valid_tile(const Vector2i& a)  {
		if (a.x >= tilemap::WIDTH || a.x < 0
		|| a.y >= tilemap::HEIGHT || a.y < 0
		|| (a.x + a.y * tilemap::WIDTH) >= tilemap::COUNT) 
			return false;
		else 
			return true;
	}

	static bool is_valid_index(const int& index) {
		return index < tilemap::COUNT && index >= 0;
	}
	
	static int coord_to_index(const Vector2i& a) {
		//if (!is_valid_tile(a)) return UINT32_MAX;
		assert(is_valid_tile(a));
		return (uint32)(a.x + a.y * tilemap::WIDTH);
	}

	static Vector2i index_to_coord(const int& index) {
		//if (index >= tilemap::COUNT || index < 0) return Vector2i(-1, -1);
		assert(is_valid_index(index));	
		int y = index / tilemap::WIDTH;
		int x = index % tilemap::WIDTH;
		return Vector2i(x, y);
	}

	/*static Vector2 coord_to_pos_center(const Vector2i& coord) {
		const float x = ((float)coord.x * (float)tilemap::TILE_PIXEL_LENGTH) + tilemap::TILE_PIXEL_CENTER_OFFSET.x;
		const float y = ((float)coord.y * (float)tilemap::TILE_PIXEL_LENGTH) + tilemap::TILE_PIXEL_CENTER_OFFSET.y;

		return Vector2(x, y);
	}*/

	static Vector2 coord_to_pos_center(const Vector2i& coord) {
		return Vector2((float)coord.x, (float)coord.y) + Vector2(0.5, 0.5);
	}



	// Player-user state, to keep track of game player slots.
	struct player_info {
		static constexpr uint32 NAME_LENGTH_MAX = 12;

		enum class status : uint8 {	// NOTE: These are also used by the send system to give disconnect reason
			EMPTY,
			JOINING,
			ACTIVE,
			ACTIVE_BOT,
			AFK,
			LOSER,
			WINNER,
			RAGEQUIT,
			USER_LEFT,
			KICKED,
			TIMED_OUT,
			SERVER_TIMED_OUT,
			MAX
		};


		static std::string status_to_str(const status& s) {
			assert((int)s >= 0 && s < status::MAX);
			switch (s) {
			case status::EMPTY:		return std::string("EMPTY");
			case status::JOINING:	return std::string("JOINING");
			case status::ACTIVE:	return std::string("ACTIVE");
			case status::ACTIVE_BOT: return std::string("ACTIVE_BOT");
			case status::AFK:		return std::string("AFK");
			case status::LOSER:		return std::string("LOSER");
			case status::WINNER:	return std::string("WINNER");
			case status::RAGEQUIT:	return std::string("RAGEQUIT");
			case status::USER_LEFT: return std::string("USER_LEFT");
			case status::KICKED:	return std::string("KICKED");
			case status::TIMED_OUT: return std::string("TIMED_OUT");
			case status::SERVER_TIMED_OUT: return std::string("SERVER_TIMED_OUT");
			default: return std::string("OUT_OF_RANGE STATUS");
			}
		};

		player_info() = default;

		//bool m_is_host	// noone is host since the server is running on a sepparate exe. 
							// The server exe could have admin tools, if needed.
		status m_player_status = status::EMPTY;
		char   m_name[NAME_LENGTH_MAX] = "";				// TODO Replace with std::string or similar
		//float  m_rtt = 0;
	};

	struct player_entity {
		static constexpr double MOVE_SPEED = 5;

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
		uint32  m_prev_action_tick = 0;		// The tick the player client input that action. Used to know how delayed input is, and what local input has been "used"
		Vector2	m_position = {};
	};

#ifdef _SERVER
	struct player_action_queue {
		static constexpr int SIZE = 3;		// NOTE: The point of this queue is to allow multiple input to be stretched over a longer period sent at 
												// the same time, but this inherently means adding delay. 
												// If we could afford increasing Network send-rate to match game tickrate, it would be avoided
		
		player_action_queue() = default;
		
		bool is_empty() const { return m_size == 0; }
		

		void consume_next() {
			assert(m_size > 0);
			const int newest = (int)m_size - 1;
			for (int i = 0; i < newest; i++) {
				m_actions[i] = m_actions[i + 1];
				m_ticks[i] = m_ticks[i + 1];
			}
			// Since everything was moved down a step, the last becomes empty
			m_actions[newest] = {};
			m_ticks[newest] = {};
			m_size--;
		}

		// Reads oldest value and consumes it
		std::pair<player_entity::action, uint32> read_next() {
			assert(!is_empty());

			std::pair<player_entity::action, uint32> r = std::pair<player_entity::action, uint32>(m_actions[0], m_ticks[0]);
			consume_next();

			return r;
		}

		// Adds new to the last index, removing the oldest element to make space (if full)
		void append_new(player_entity::action action, uint32 tick) {
			while (m_size >= SIZE) consume_next();
			
			if (m_size < SIZE) {
				m_actions[m_size] = action;
				m_ticks[m_size] = tick;
				m_size++;
			}
			return;
		}

	private:	// These properties and elements are not invalidated / reset when not in-queue, so we don't let it be checked
		uint32 m_size = 0;
		player_entity::action m_actions[SIZE] = {};
		uint32 m_ticks[SIZE] = {};
	};
#endif // _SERVER

	struct bomb {
		static constexpr double FUSE_TIME = 1.5;
		static constexpr uint32 FUSE_TICKS = (uint32)(FUSE_TIME * (double)TICK_RATE);
		static constexpr uint32 COOLDOWN_TICKS = (uint32)(0.3 * (double)TICK_RATE);

		bomb() = default;
		bomb(uint8 x, uint8 y, int32 explosion_tick);
		uint8   m_x = 0;
		uint8   m_y = 0;
		uint32	m_explosion_tick = 0;
	};




	struct game_state {
		game_state() = default;

		player_entity m_players[MAX_PLAYERS] = {};
		bomb		  m_bombs[MAX_PLAYERS] = {};
		tilemap		  m_tilemap = {};
		uint32		  m_tick = 0;

		const player_entity& get_player(const int index) const;
		const bomb& get_bomb(const int index) const;
		const tilemap& get_tilemap() const;
		const bool is_default() const;

		bool is_walkable(const Vector2i& coord) const;
		void update_player(const uint8& player_index);
		bool can_place_bomb(const uint8& index) const;
		bool is_bomb_at(const Vector2i& coord) const noexcept;
		void apply_bomb_explosion(const bomb& da_bomb);
		bool explode_at(const Vector2i& coord);
	};


	struct game {
		//static constexpr int ACTIONS_BUFFER_LENGTH = 12;
		static constexpr int STATE_HISTORY_LENGTH = 30;
		static constexpr int STATE_QUEUE_LENGTH = 6;
		
		static const Vector2i GET_PLAYER_START_TILE(const int& i) {
			
			static const Vector2i PLAYER_START_COORDS[MAX_PLAYERS] = {
				Vector2i(0, 0),
				Vector2i((int)tilemap::WIDTH - 1, (int)tilemap::HEIGHT - 1),
				Vector2i((int)tilemap::WIDTH - 1, 0),
				Vector2i(0, (int)tilemap::HEIGHT - 1)
			};
			return PLAYER_START_COORDS[i];
		};

		enum class status : uint8 {
			INVALID,
			PRE_GAME,
			IN_GAME,
			POST_GAME
		};
		
		game() = default;
		void init();
		void start();

		uint32 get_player_count() const;
		void fill_player_slots_with_bots();

		player_info m_player_info[MAX_PLAYERS] = {};

		game_state  m_state = {};
		//uint32	m_tick = 0;
		status		m_status = status::INVALID;

#ifdef _CLIENT
		int	m_user_index = -1;								// index of local user client

		std::vector<player_entity::action> m_predict_actions = std::vector<player_entity::action>();
		mutable uint8 m_actions_not_sent = 0;				// Used by the send system to know what is queued. Mutable so send system can modify despite const refrence, for type safety

		game_state m_predicted_state = {};							// Result state from m_state having predicted actions applied.
		game_state m_prev_state = {};								// Previously played state, used for split-frame interpolation (if needed)

		std::vector<game_state> m_state_queue = std::vector<game_state>();

#endif

#ifdef _SERVER
		void push_state_to_history();
		std::vector<game_state> m_state_history = std::vector<game_state>();
		player_action_queue m_player_action_queue[MAX_PLAYERS] = {};
		mutable int  m_states_not_sent = 0;
		mutable bool m_game_lobby_changed = false;
		mutable bool m_queue_game_start = false;			// If the game should start next send tick
#endif
		
	};


	static void interp_game_states(const game_state& from, const game_state& to, std::vector<game_state>& result_arr) {
		if (to.m_tick - from.m_tick <= 0) { return; }

		// Skip 0 since that is just the 'from' state
		for (int i = 1; i < (int)to.m_tick; i++) {
			const float fraction = (float)(i) / (float)(to.m_tick);
			game_state state = game_state(from);
			state.m_tick += i;

			int plr_i = 0;
			for (player_entity& player : state.m_players) {
				player.m_position = Vector2Lerp(player.m_position, to.m_players[plr_i].m_position, fraction);
				plr_i++;
			}
			result_arr.push_back(state);
		}
	}

	
}
