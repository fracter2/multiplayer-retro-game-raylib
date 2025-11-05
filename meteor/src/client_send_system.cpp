// client_send_system.cpp

#pragma once

#include "client_send_system.hpp"


namespace meteor::client_send_system {


	// TODO Refrence connection status (const), game state (const)
	// TODO QUEUE SEND DATA, INPUT ACTIONS, WRAP IN TICK CLOSURE
	// note: network send update
	void update(const uint32& ticks, const double time, udp_socket& socket, connection& conn, const ip_endpoint& client_endpoint, const ip_endpoint& server_endpoint, const game::game& game_instance) {

		// Only perform send update once every third tick
		if (ticks % TICKS_PER_UPDATE != 0) {
			return;
		}


		byte_stream stream_send;
		byte_stream_writer writer(stream_send);

		switch (conn.m_status) {
		case connection::status::CONNECTING: {
			debug::info("sending connect package");
			connect_packet packet;
			packet.write(writer);

			if (!socket.send_to(server_endpoint, stream_send)) { print_error_code(); }
			else {
				// TODO LOG DATA SENT
			}

			break;
		}

		case connection::status::CONNECTED: {

			conn.m_send_sequence += 1;
			payload_packet packet(conn.m_send_sequence, conn.m_recieve_sequence);
			packet.write(writer);


			if (game_instance.m_status != game::game::status::IN_GAME) {
				if (!socket.send_to(server_endpoint, stream_send)) { print_error_code(); }
				else {
					// TODO LOG DATA SENT
				}

				break;
			}


			// TODO SEND USERS CLIENT using PLAYER_ID IN SERVER

			// Type safe const to reduce word lengths and to emphasise when it's mutable or not (to avoid setting accidently)
			const int				   user_index  = game_instance.m_user_index;
			const uint32			   tick		   = game_instance.m_tick;
			const game::game_state&	   state	   = game_instance.m_state;
			const game::player_entity& user_player = state.get_player(user_index);


			// Send unsent input (only unsent input as server ignores older input anyway)
			int unsent_action_i = (int)game_instance.m_predict_actions.size() - (int)game_instance.m_actions_not_sent;

			for (int i = unsent_action_i; i < game_instance.m_predict_actions.size(); i++) {
				const uint32 action_tick = game_instance.m_tick - (game_instance.m_actions_not_sent + 1);					// +1 so when only one is unsent, it's the current tick (as it is)
				input_action_message message = input_action_message(game_instance.m_predict_actions[i], action_tick);

				if (writer.m_stream.can_fit(sizeof(message))) {	// LIMIT TO MAX PACKAGE SIZE
					message.write(writer);
					game_instance.m_actions_not_sent -= 1;
				}
				else { break; }
				
			}
			
			debug::info("sending payload package");
			if (!socket.send_to(server_endpoint, stream_send)) { print_error_code(); }
			else {
				// TODO LOG DATA SENT
			}
			

			break;
		}

		case connection::status::DISCONNECTED: {

			// TODO Send mouse pos, key presses, system resources, language, location, windows activation code, clipboard, screen image buffer etc to corp. customer-safety-and-satisfaction server (CS_ASS)
			
			break;
		}


		case connection::status::DISCONNECTING: {
			
			break;
		}

		}// !switch (server_connection.m_status)


	
	}// !network send update


}

