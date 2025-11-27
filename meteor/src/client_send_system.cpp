// client_send_system.cpp

#pragma once

#include "client_send_system.hpp"




#define SAFE_WRITE(message)																	\
if (writer.m_stream.can_fit(sizeof(message)))	{										\
	message.write(writer);																\
}																							\
else {																						\
	debug::warn("%g - message cannot fit!, size: %d", GetTime(), stream_send.size());			\
	break;																						\
}

namespace meteor::client_send_system {

	static void send_discovery(
		udp_socket& socket,
		server_browser& browser,
		const game& game_instance) {

		byte_stream stream_send;
		byte_stream_writer writer(stream_send);
		ip_endpoint broadcast_endpoint = ip_endpoint(network::get_broadcast_address(), PORT);

		discovery_packet packet = discovery_packet(0, false);
		packet.write(writer);

		debug::info("%g - sending discovery broadcast", GetTime());
		if (!socket.send_to(broadcast_endpoint, stream_send)) { print_error_code(); }
		else {
			// TODO LOG DATA SENT
		}
	}

	void update(const uint32& ticks, udp_socket& socket, connection& conn, const game& game_instance, server_browser& browser) {

		// Only perform send update once every third tick
		if (ticks % TICKS_PER_NETWORK_SEND != 0) {
			return;
		}

		conn.increment_recieve_history();
		conn.increment_send_history();

		byte_stream stream_send;
		byte_stream_writer writer(stream_send);

		switch (conn.get_status()) {
		case connection::status::CONNECTING: {
			debug::info("sending connect package");
			connect_packet packet;
			packet.write(writer);

			conn.send_stream(socket, stream_send);
			break;
		}

		case connection::status::CONNECTED: {

			payload_packet packet(conn.get_send_sequence() + 1, conn.get_recieve_sequence());
			packet.write(writer);

			if (game_instance.m_status != game::status::IN_GAME) {
				conn.send_payload(socket, stream_send);
				break;
			}

			// Type safe const to reduce word lengths and to emphasise when it's mutable or not (to avoid setting accidently)
			const int			 user_index  = game_instance.m_user_index;
			const uint32		 tick		 = game_instance.m_state.m_tick;
			const game_state&	 state	     = game_instance.m_state;
			const player_entity& user_player = state.get_player(user_index);


			// Send unsent input (only unsent input as server ignores older input anyway)
			int unsent_action_i = (int)game_instance.m_predict_actions.size() - (int)game_instance.m_actions_not_sent;
			assert(unsent_action_i >= 0);

			// Oldest first
			for (int i = unsent_action_i; i < game_instance.m_predict_actions.size(); i++) {
				const player_entity::action& action = game_instance.m_predict_actions[i];
			//for (const player_entity::action& action : game_instance.m_predict_actions) {

				const uint32 action_tick = tick - (game_instance.m_actions_not_sent - 1);					// +1 so when only one is unsent, it's the current tick (as it is)
				input_action_message message = input_action_message(action, action_tick);

				SAFE_WRITE(message);
				game_instance.m_actions_not_sent -= 1;
				
			}
			
			conn.send_payload(socket, stream_send);
			

			break;
		}

		case connection::status::DISCONNECTED: {

			// TODO Send mouse pos, key presses, system resources, language, location, windows activation code, clipboard, screen image buffer etc to corp. customer-safety-and-satisfaction server (CS_ASS)
			
			if (browser.m_queue_discovery_send) {
				browser.m_queue_discovery_send = false;
				send_discovery(socket, browser, game_instance);
			}

			break;
		}


		case connection::status::DISCONNECTING: {
			
			debug::info("sending disconnect package");
			disconnect_packet packet;
			packet.write(writer);

			conn.send_stream(socket, stream_send);

			break;
		}

		}// !switch (server_connection.m_status)


	
	}// !network send update


}

