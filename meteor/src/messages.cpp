// messages.cpp

#include "messages.hpp"

namespace meteor
{
	// ---- game_state_message ----
	

	game_state_message::game_state_message(game::game_state state)
		: m_type(message_type::GAME_STATE)
		, m_game_state(state)
	{
	}

	bool game_state_message::write(byte_stream_writer& writer) { return serialize(*this, writer); }
	bool game_state_message::read(byte_stream_reader& reader) { return serialize(*this, reader); }

	template <typename T>
	bool serialize(game_state_message& message, T& stream)
	{
		bool success = true;
		success &= stream.serialize(message.m_type);
		success &= stream.serialize(message.m_game_state);
		return success;
	}

	template <typename T>
	bool serialize(game::game_state& state, T& stream) {
		bool success = true;
		for (int i = 0; i < game::MAX_PLAYERS; i++) {
			success &= stream.serialize(state.m_bombs[i]);
			success &= stream.serialize(state.m_players[i]);
		}
		success &= stream.serialize(state.m_tilemap);

	}

	template <typename T>
	bool serialize(game::player_entity& player, T& stream) {
		bool success = true;
		success &= stream.serialize(player.m_dead);
		success &= stream.serialize(player.m_position.x);
		success &= stream.serialize(player.m_position.y);
		success &= stream.serialize(player.m_velocity.x);
		success &= stream.serialize(player.m_velocity.y);
		return success;
	}

	template <typename T>
	bool serialize(game::bomb& bomb, T& stream) {
		bool success = true;
		
		success &= stream.serialize(bomb.m_explosion_tick);
		success &= stream.serialize(bomb.m_position.x);
		success &= stream.serialize(bomb.m_position.y);
		return success;
	}




	// ---- input_action_message ----

	bool input_action_message::write(byte_stream_writer& writer) { return serialize(*this, writer); }
	bool input_action_message::read(byte_stream_reader& reader) { return serialize(*this, reader); }

	input_action_message::input_action_message(game::player_actions action) 
		: m_type(message_type::INPUT_ACTION)
		, m_action(action)
	{
	}

   template <typename T>
   bool serialize(input_action_message& message, T& stream) {
	   bool success = true;
	   success &= stream.serialize(message.m_type);
	   success &= stream.serialize(player.m_position.x);
	   success &= stream.serialize(player.m_position.y);
	   success &= stream.serialize(player.m_velocity.x);
	   success &= stream.serialize(player.m_velocity.y);
	   return success;
   }

   
   

} // !meteor
