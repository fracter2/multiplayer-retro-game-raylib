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


	// ---- game_state_message ----

	template <typename T>
	bool serialize(bomb& bomb, T& stream) {
		bool success = true;
		
		success &= stream.serialize(bomb.m_explosion_tick);
		success &= stream.serialize(bomb.m_x);
		success &= stream.serialize(bomb.m_y);
		return success;
	}

	template <typename T>
	bool serialize(player_entity& player, T& stream) {
		bool success = true;
		success &= stream.serialize(player.m_dead);
		success &= stream.serialize(player.m_position.x);
		success &= stream.serialize(player.m_position.y);
		success &= stream.serialize(player.m_prev_action_tick);
		//success &= stream.serialize(player.m_prev_action);
		success &= serialize<player_entity::action, uint8>(player.m_prev_action, stream);
		return success;
	}

	template <typename T>
	bool serialize(game_state& state, T& stream) {
		bool success = true;
		for (int i = 0; i < MAX_PLAYERS; i++) {
			success &= serialize(state.m_bombs[i], stream);
			success &= serialize(state.m_players[i], stream);
		}
		//success &= stream.serialize(state.m_tilemap);

		//for (uint8 tile : state.m_tilemap.m_tiles) {
		//success &= stream.serialize(tile);
		//}
		for (int i = 0; i < tilemap::BYTES_NEEDED; i++) {
			success &= stream.serialize(state.m_tilemap.m_tiles[i]);
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


	game_state_message::game_state_message(game_state state, uint32 tick)
		: m_type(message_type::GAME_STATE)
		, m_game_state(state)
		, m_tick(tick)
	{
	}


	// ---- input_action_message ----

	template <typename T>
	bool serialize(input_action_message& message, T& stream) {
		bool success = true;
		success &= serialize<message_type, uint8>(message.m_type, stream);
		success &= stream.serialize(message.m_tick);
		//success &= stream.serialize(message.m_action);
		success &= serialize<player_entity::action, uint8>(message.m_action, stream);
		return success;
	}

	bool input_action_message::write(byte_stream_writer& writer) { return serialize(*this, writer); }
	bool input_action_message::read(byte_stream_reader& reader) { return serialize(*this, reader); }

	input_action_message::input_action_message(player_entity::action action, uint32 tick) 
		: m_type(message_type::INPUT_ACTION)
		, m_action(action)
		, m_tick(tick)
	{
	}



	// ---- game_lobby_message ----

	template <typename T>
	bool serialize(player_info& info, T& stream) {
		bool success = true;
		success &= serialize<player_info::status, uint8>(info.m_player_status, stream);
		for (char& c : info.m_name) {
			success &= stream.serialize(c);
		}
		
		return success;
	}

	template <typename T>
	bool serialize(game_lobby_message& message, T& stream) {
		bool success = true;
		success &= serialize<message_type, uint8>(message.m_type, stream);
		success &= stream.serialize(message.m_start_now);
		for (player_info& info : message.m_player_info) { serialize(info, stream); }
		success &= serialize<game::status, uint8>(message.m_game_status, stream);

		return success;
	}

	bool game_lobby_message::write(byte_stream_writer& writer) { return serialize(*this, writer); }
	bool game_lobby_message::read(byte_stream_reader& reader) { return serialize(*this, reader); }

	// TODO Learn how to properly use iterators... or just use pointer...
	game_lobby_message::game_lobby_message(bool start_now, const game& game_instance, const game::status& status)
		: m_type(message_type::GAME_LOBBY)
		, m_start_now(start_now)
		, m_game_status(status)
	{
		for (int i = 0; i < MAX_PLAYERS; i++){
			m_player_info[i] = game_instance.m_player_info[i];
		}
		
	}
   
   

} // !meteor
