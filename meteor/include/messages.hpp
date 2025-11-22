// messages.hpp

#pragma once

#include "raylib.h"
#include "network.hpp"
#include "game.hpp"

namespace meteor
{
	// NOTE: When serialised into byte stream, the messages take less space due to their alignment not making "gaps" in allocataded space

   enum class message_type : uint8 {
	  GAME_STATE,
	  //GAME_DELTA,
	  //TICK_WRAP,		// Used to state the tick for the following messages	// TODO CONSIDER REMOVING we can just add tick to message...
	  //SEQUENCE_WRAP,	// Used to state the sequence for following messages, (for reliable messages client can check if it already recieved the induvidual message)
	  INPUT_ACTION,
	  GAME_LOBBY,
	  MAX
   };

   struct game_state_message {
	   //game_state_message() = default;
	   game_state_message() = default;
	   game_state_message(game_state state);

	   bool write(byte_stream_writer& writer);
	   bool read(byte_stream_reader& reader);

	   message_type	m_type = message_type::GAME_STATE;
	   game_state   m_game_state = {};
   };

   struct input_action_message {
	   input_action_message() = default;
	   input_action_message(player_entity::action action, uint32 tick);

	   bool write(byte_stream_writer& writer);
	   bool read(byte_stream_reader& reader);

	   message_type			 m_type = message_type::INPUT_ACTION;
	   uint32				 m_tick = 0;
	   player_entity::action m_action = player_entity::action::INVALID;
   };

   struct game_lobby_message {
	   game_lobby_message() = default;
	   game_lobby_message(bool start_now, const game& game_instance, const game::status& status);

	   bool write(byte_stream_writer& writer);
	   bool read(byte_stream_reader& reader);

	   message_type	m_type = message_type::GAME_LOBBY;
	   bool			m_start_now = false;
	   player_info  m_player_info[MAX_PLAYERS] = {};
	   game::status m_game_status = game::status::INVALID;
	   // messages? with ack confirmation in send system?
   };

} // !meteor
