// messages.cpp

# pragma once
#include "messages.hpp"

namespace meteor
{
	// This is to convert from enum class types through the stream writers (or any type that can be cast with () )
	// This is needed since their "std::is_fundamental()" check doesn't allow enum classes, and casting alone
	// doesn't work since the reader and writer need opposite casts.
	// Otherwise, we would have to make specific "stream.serialize(this_specific_type)" for every exception
	// or remake all enum class to their base type.
    template <typename T_from, typename T_to>
	bool serialize(T_from& type, byte_stream_writer& stream)
	{
		bool success = true;
		success &= stream.serialize((T_to)type);
		return success;
	}

	template <typename T_from, typename T_to>
	bool serialize(T_from& type, byte_stream_reader& stream)
	{
		bool success = true;
		T_to v = 0;
		success &= stream.serialize(v);
		type = (T_from)v;
		return success;
	}
	/*
	bool serialize(message_type& type, byte_stream_writer& stream)
	{
		bool success = true;
		success &= stream.serialize((uint8)type);
		return success;
	}
	
	bool serialize(message_type& type, byte_stream_reader& stream)
	{
		bool success = true;
		uint8 v = 0;
		success &= stream.serialize(v);
		type = (message_type)v;
		return success;
	}*/

	// ---- game_state_message ----
	

	template <typename T>
	bool serialize(game::bomb& bomb, T& stream) {
		bool success = true;
		
		success &= stream.serialize(bomb.m_explosion_tick);
		success &= stream.serialize(bomb.m_x);
		success &= stream.serialize(bomb.m_y);
		return success;
	}

	template <typename T>
	bool serialize(game::player_entity& player, T& stream) {
		bool success = true;
		success &= stream.serialize(player.m_dead);
		success &= stream.serialize(player.m_position.x);
		success &= stream.serialize(player.m_position.y);
		//success &= stream.serialize(player.m_prev_action);
		success &= serialize<game::player_entity::action, uint8>(player.m_prev_action, stream);
		return success;
	}

	template <typename T>
	bool serialize(game::game_state& state, T& stream) {
		bool success = true;
		for (int i = 0; i < game::MAX_PLAYERS; i++) {
			//success &= stream.serialize(state.m_bombs[i]);
			//success &= stream.serialize(state.m_players[i]);
			success &= serialize(state.m_bombs[i], stream);
			success &= serialize(state.m_players[i], stream);
		}
		//success &= stream.serialize(state.m_tilemap);
		for (uint8 tile : state.m_tilemap.m_tiles) {
			success &= stream.serialize(tile);
		}

		return success;
	}



	template <typename T>
	bool serialize(game_state_message& message, T& stream)
	{
		bool success = true;
		//success &= stream.serialize(message.m_type);
		success &= serialize<message_type, uint8>(message.m_type, stream);
		success &= stream.serialize(message.m_tick);
		//success &= stream.serialize(message.m_game_state);
		success &= serialize(message.m_game_state, stream);
		return success;
	}

	bool game_state_message::write(byte_stream_writer& writer) { return serialize(*this, writer); }
	bool game_state_message::read(byte_stream_reader& reader) { return serialize(*this, reader); }

	game_state_message::game_state_message(game::game_state state)
		: m_type(message_type::GAME_STATE)
		, m_game_state(state)
	{
	}


	// ---- input_action_message ----

	template <typename T>
	bool serialize(input_action_message& message, T& stream) {
		bool success = true;
		success &= serialize<message_type, uint8>(message.m_type, stream);
		success &= stream.serialize(message.m_tick);
		//success &= stream.serialize(message.m_action);
		success &= serialize<game::player_entity::action, uint8>(message.m_action, stream);
		return success;
	}

	bool input_action_message::write(byte_stream_writer& writer) { return serialize(*this, writer); }
	bool input_action_message::read(byte_stream_reader& reader) { return serialize(*this, reader); }

	input_action_message::input_action_message(game::player_entity::action action) 
		: m_type(message_type::INPUT_ACTION)
		, m_action(action)
	{
	}



   
   

} // !meteor
