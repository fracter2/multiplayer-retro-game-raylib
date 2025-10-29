// messages.hpp

#pragma once

#include "raylib.h"
#include "network.hpp"
#include "game.hpp"

namespace meteor
{
   enum class message_type : uint8 {
	  GAME_STATE,
	  //GAME_DELTA,
	  //TICK_WRAP,	// Used to state the tick for the following messages	// TODO CONSIDER REMOVING we can just add tick to message...
	  INPUT_ACTION
   };

   struct game_state_message {
	   //game_state_message() = default;
	   game_state_message(game::game_state state);

	   bool write(byte_stream_writer& writer);
	   bool read(byte_stream_reader& reader);

	   message_type		m_type = message_type::GAME_STATE;
	   game::game_state m_game_state = {};
   };

   struct input_action_message {
	   //game_state_message() = default;
	   input_action_message(game::player_actions action);

	   bool write(byte_stream_writer& writer);
	   bool read(byte_stream_reader& reader);

	   message_type			m_type = message_type::INPUT_ACTION;
	   game::player_actions m_action = game::player_actions::INVALID;
   };

   

} // !meteor
