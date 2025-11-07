// game.cpp

#pragma once
#include "game.hpp"



namespace meteor {

	player_entity::player_entity(Vector2 position)
		: m_position(position)
	{
	};

	bool game_state::can_place_bomb(const uint8& index, const uint32& tick) const {
		const player_entity& user_player = m_players[index];
		uint8 x, y = 0;
		vec2_to_coord(user_player.m_position, x, y);

		bool r = true;
		r &= !user_player.m_dead;							// if not player dead...
		r &= is_walkable(x, y);								// and is inside map... and is not wall...

		for (const bomb& bomb : m_bombs) {					// and no other bombs are there...
			r &= !(bomb.m_x == x
				&& bomb.m_y == y
				&& bomb.m_explosion_tick > tick);
		}
		r &= (get_bomb(index).m_explosion_tick
			+ bomb::COOLDOWN_TICKS) < tick;					// and the bomb isn't already placed or in cooldown...

		return r;
	}

	bool game_state::is_walkable(const uint8& x, const uint8& y) const {
		if (valid_tile(x, y) && !m_tilemap.is_tile_active(x, y)) 
			return false;
		else return true;
	}

	bool game_state::is_walkable(const Vector2& pos) const {
		uint8 x, y = 0;
		vec2_to_coord(pos, x, y);
		return is_walkable(x, y);
	}

	Vector2 game_state::move_and_collide(const Vector2& pos, const Vector2& vel) const {
		if (is_walkable(Vector2(pos + vel))) return Vector2(pos + vel);
		else return pos;
	}

	void game_state::apply_player_action(const uint8& player_index, const double& dt, const uint32& tick) {
		const double speed = player_entity::MOVE_SPEED * dt;	// CONSIDER USING STANDARDIZED 1/TICKRATE 
		player_entity& player = m_players[player_index];
		
		switch (player.m_prev_action) {
		case player_entity::action::INVALID: {
			// lol
			break;
		}
		case player_entity::action::STAND_STILL: {
			// lol
			break;
		}
		case player_entity::action::MOVE_UP: {
			player.m_position = move_and_collide(player.m_position, Vector2(0, -1) * (float)speed);
			
			break;
		}
		case player_entity::action::MOVE_DOWN: {
			player.m_position = move_and_collide(player.m_position, Vector2(0, 1) * (float)speed);
			break;
		}
		case player_entity::action::MOVE_LEFT: {
			player.m_position = move_and_collide(player.m_position, Vector2(-1, 0) * (float)speed);
			break;
		}
		case player_entity::action::MOVE_RIGHT: {
			player.m_position = move_and_collide(player.m_position, Vector2(1, 0) * (float)speed);
			break;
		}
		case player_entity::action::PLACE_BOMB: {		// TODO CONSIDER SETTING EXPLOSION TIME BASED ON INPUT-TICK player.m_prev_action_tick
			bomb& da_bomb = m_bombs[player_index];
			uint8 x, y = 0;
			vec2_to_coord(player.m_position, x, y);
			if (can_place_bomb(player_index, tick)) {
				da_bomb.m_explosion_tick = tick + bomb::FUSE_TICKS;
				da_bomb.m_x = x;
				da_bomb.m_y = y;
			}
			break;
		}
			
		}
	}

	// Returns true if it hit a tile, destroying it, and killingany players on the way
	bool game_state::apply_bomb_explosion_to_tile(const bomb& da_bomb, const uint8& x, const uint8& y) {
		for (player_entity& player : m_players) {
			if (!player.m_dead) {
				uint8 m_x, m_y = 0;
				vec2_to_coord(player.m_position, m_x, m_y);
				if (m_x == x && m_y == da_bomb.m_y) { player.m_dead = true; }
			}
		}
		if (m_tilemap.is_tile_active(x, y)) { 
			m_tilemap.set_tile(x, y, false); 
			return true; 
		}
		return false;
	}

	void game_state::apply_bomb_explosion(const uint8& index) {
		bomb& da_bomb = m_bombs[index];

		// Traverse left, right, up and down. Kill any players in the way, and destroy the first hit tile (in each direction)
		for (uint8 x = da_bomb.m_x; x < tilemap::WIDTH; x++) {
			if (apply_bomb_explosion_to_tile(da_bomb, x, da_bomb.m_y)) break;
		}
		for (uint8 x = da_bomb.m_x; x > 0; x--) {
			if (apply_bomb_explosion_to_tile(da_bomb, x - 1, da_bomb.m_y)) break;
		}
		for (uint8 y = da_bomb.m_y; y < tilemap::HEIGHT; y++) {
			if (apply_bomb_explosion_to_tile(da_bomb, da_bomb.m_x, y)) break;
		}
		for (uint8 y = da_bomb.m_y; y > 0; y--) {
			if (apply_bomb_explosion_to_tile(da_bomb, da_bomb.m_x, y - 1)) break;
		}
	}

	bomb::bomb(uint8 x, uint8 y, int32 explosion_tick)
		: m_x(x)
		, m_y(y)
		, m_explosion_tick(explosion_tick)
	{
	}

	bool tilemap::is_tile_active(const uint8 x, const uint8 y) const {
		assert(valid_tile(x, y));

		uint8 byte = *(m_tiles + ((x + y * WIDTH) / 8));
		uint8 bitmask = (uint8)1 << ((x + y * WIDTH) % 8);

		return (byte & bitmask) != 0;	// & is the bitwise "and" operator, so if the result is higher than 0, that bit is active.
	}

	bool tilemap::is_tile_active(const uint32 index) const {
		assert(index < tilemap::COUNT);

		uint8 byte = *(m_tiles + (index / 8));
		uint8 bitmask = (uint8)1 << (index % 8);

		return (byte & bitmask) != 0;	// & is the bitwise "and" operator, so if the result is higher than 0, that bit is active.
	}

	void tilemap::set_tile(const uint8 x, const uint8 y, bool value) {
		assert(valid_tile(x, y));
		uint8& byte = *(m_tiles + ((x + y * WIDTH) / 8));
		uint8 bitmask = (uint8)1 << ((x + y * WIDTH) % 8);

		if (value) { byte = byte | bitmask; }			// | is bitwise "or", resulting in all 1s being kept from both
		else { byte = byte & (~bitmask); }		// ~ is bitwise complement operator, flipping all bits 1->0 and 0->1
	}

	const player_entity& game_state::get_player(const int index) const {
		assert(index < MAX_PLAYERS && index >= 0);
		if (index >= MAX_PLAYERS) return m_players[MAX_PLAYERS - 1];
		else if (index < 0)		  return m_players[0];
		else					  return m_players[index];
	}

	const bomb& game_state::get_bomb(const int index) const {
		assert(index < MAX_PLAYERS && index >= 0);
		if (index >= MAX_PLAYERS) return m_bombs[MAX_PLAYERS - 1];
		else if (index < 0)		  return m_bombs[0];
		else					  return m_bombs[index];
	}

	const tilemap& game_state::get_tilemap() const { return m_tilemap; }
	const bool game_state::is_default() const {
		return m_players[0].m_prev_action == player_entity::action::INVALID;
	}
}