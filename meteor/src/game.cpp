// game.cpp

#pragma once
#include "game.hpp"



namespace meteor::game {

	player_entity::player_entity(Vector2 position)
		: m_position(position)
	{
	};

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