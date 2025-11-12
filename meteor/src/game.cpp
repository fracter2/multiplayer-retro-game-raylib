// game.cpp

#pragma once
#include "game.hpp"



namespace meteor {

	void game::init() {

		m_state.m_tilemap.set_all(true);

		int i = 0;
		for (player_entity& player : m_state.m_players) {
			if (m_player_info[i].m_player_status == player_info::status::ACTIVE) {
				player.m_dead = false;
				player.m_prev_action = player_entity::action::STAND_STILL;
				const Vector2i coord = GET_PLAYER_START_TILE(i);
				player.m_position = coord_to_pos_center(coord);

				m_state.m_tilemap.set_tile(coord, false);
				if (is_valid_tile(coord + Vector2i_UP))		m_state.m_tilemap.set_tile(coord + Vector2i_UP, false);
				if (is_valid_tile(coord + Vector2i_DOWN))	m_state.m_tilemap.set_tile(coord + Vector2i_DOWN, false);
				if (is_valid_tile(coord + Vector2i_LEFT))	m_state.m_tilemap.set_tile(coord + Vector2i_LEFT, false);
				if (is_valid_tile(coord + Vector2i_RIGHT))	m_state.m_tilemap.set_tile(coord + Vector2i_RIGHT, false);
			}
			i++;
		}
		
		
		
		// TODO set gameplay mode
	}

	player_entity::player_entity(Vector2 position)
		: m_position(position)
	{
	};

	bool game_state::can_place_bomb(const uint8& index, const uint32& tick) const {
		const player_entity& user_player = m_players[index];

		Vector2i coord = Vector2i(user_player.m_position);

		bool r = true;
		r &= !user_player.m_dead;							// if not player dead...
		r &= is_walkable(coord);							// and is inside map...

		for (const bomb& bomb : m_bombs) {					// and no other bombs are there...
			r &= !(bomb.m_x == coord.x
				&& bomb.m_y == coord.y
				&& bomb.m_explosion_tick > tick);
		}
		r &= (get_bomb(index).m_explosion_tick
			+ bomb::COOLDOWN_TICKS) < tick;					// and the bomb isn't already placed or in cooldown...

		return r;
	}

	bool game_state::is_walkable(const Vector2i& coord) const {
		if (is_valid_tile(coord)) 
			if (!m_tilemap.is_tile_active(coord)) 
				return true;
		return false;
	}

	/*Vector2 game_state::move_and_collide(const Vector2& pos, const Vector2& vel) const {
		if (is_walkable(Vector2(pos + vel))) return Vector2(pos + vel);
		else return pos;
	}*/

	void game_state::update_player(const uint8& player_index, const uint32& tick) {
		const double speed = player_entity::MOVE_SPEED * TICK_TIME;
		player_entity& player = m_players[player_index];

		Vector2 dir = { 0, 0 };

		switch (player.m_prev_action) {
		case player_entity::action::INVALID: {		// Is this state really usefull?
			// lol
			break;
		}
		case player_entity::action::STAND_STILL: {
			// lol
			break;
		}
		case player_entity::action::MOVE_UP: {
			dir = Vector2(0, -1);
			break;
		}
		case player_entity::action::MOVE_DOWN: {
			dir = Vector2(0, 1);
			break;
		}
		case player_entity::action::MOVE_LEFT: {
			dir = Vector2(-1, 0);
			break;
		}
		case player_entity::action::MOVE_RIGHT: {
			dir = Vector2(1, 0);
			break;
		}
		case player_entity::action::PLACE_BOMB: {		// TODO CONSIDER SETTING EXPLOSION TIME BASED ON INPUT-TICK player.m_prev_action_tick
			bomb& da_bomb = m_bombs[player_index];
			Vector2i coord = Vector2i(player.m_position);
			
			if (can_place_bomb(player_index, tick)) {
				da_bomb.m_explosion_tick = tick + bomb::FUSE_TICKS;
				da_bomb.m_x = (uint8)coord.x;
				da_bomb.m_y = (uint8)coord.y;
			}
			break;
		}
		}

		if (dir != Vector2(0, 0)) {
			Vector2 new_pos = player.m_position + dir * (float)speed;
			if (is_walkable(new_pos)) {
				player.m_position = new_pos;
			}
		}

		

	}

	// Returns true if it hit a tile, destroying it, and killing any players on the way
	bool game_state::explode_at(const Vector2i& coord) {	// TODO Simplify by removing sepparate x/y, and merge or make private
		for (player_entity& player : m_players) {
			if (!player.m_dead && coord == Vector2i(player.m_position)) {
				player.m_dead = true;
			}
		}
		if (m_tilemap.is_tile_active(coord)) { 
			m_tilemap.set_tile(coord, false); 
			return true; 
		}
		return false;
	}

	void game_state::apply_bomb_explosion(const bomb& da_bomb) {

		// Goal: Traverse left, right, up and down. Kill any players in the way, and destroy the first hit tile (in each direction)
		for (int x = da_bomb.m_x; x < tilemap::WIDTH; x++)  { if (explode_at(Vector2i(x, da_bomb.m_y))) break; }
		for (int x = da_bomb.m_x; x >= 0; x--)				{ if (explode_at(Vector2i(x, da_bomb.m_y))) break; }
		for (int y = da_bomb.m_y; y < tilemap::HEIGHT; y++) { if (explode_at(Vector2i(da_bomb.m_x, y))) break; }
		for (int y = da_bomb.m_y; y >= 0; y--)				{ if (explode_at(Vector2i(da_bomb.m_x, y))) break; }

	}

	bomb::bomb(uint8 x, uint8 y, int32 explosion_tick)
		: m_x(x)
		, m_y(y)
		, m_explosion_tick(explosion_tick)
	{
	}

	bool inline tilemap::is_tile_active(const Vector2i& coord) const {
		return is_tile_active(coord_to_index(coord));
	}

	bool tilemap::is_tile_active(const int& index) const {
		assert(is_valid_index(index));
		uint8 byte = *(m_tiles + (index / 8));
		uint8 bitmask = (uint8)1 << (index % 8);

		return (byte & bitmask) != 0;	// & is the bitwise "and" operator, so if the result is higher than 0, that bit is active.
	}

	void tilemap::set_tile(const Vector2i& coord, const bool& val) {
		assert(is_valid_tile(coord));
		set_tile(coord_to_index(coord), val);
		
	}

	void tilemap::set_tile(const int& index, const bool& val) {
		assert(is_valid_index(index));
		uint8& byte = *(m_tiles + (index / 8));
		uint8 bitmask = (uint8)1 << (index % 8);

		if (val) { byte = byte | bitmask; }			// | is bitwise "or", resulting in all 1s being kept from both
		else { byte = byte & (~bitmask); }		// ~ is bitwise complement operator, flipping all bits 1->0 and 0->1
	}

	void tilemap::set_all(const bool& value) {
		for (uint8& byte : m_tiles) {
			if (value) byte = UINT8_MAX;
			else byte = 0;
		}
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