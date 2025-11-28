// server_send_system.cpp

#pragma once

#include "server_send_system.hpp"


namespace meteor::server_send_system {

	static void send_discovery_responses(
		udp_socket& socket,
		server_state& server,
		const game& game_instance) {
	
		for (const ip_endpoint& endp : server.m_discovery_response_queue) {

			byte_stream stream_send;
			byte_stream_writer writer(stream_send);

			discovery_packet packet = discovery_packet((uint8)server.get_client_count(), true);
			packet.write(writer);

			debug::info("%g - sending discovery response", GetTime());
			if (!socket.send_to(endp, stream_send)) { print_error_code(); return; }
			else {
				// TODO LOG DATA SENT
			}

		}

		server.m_discovery_response_queue.clear();
	}


#define SAFE_WRITE(message)																	\
if (writer.m_stream.can_fit(sizeof(message)))	{										\
	message.write(writer);																\
}																							\
else {																						\
	debug::warn("%g - message cannot fit!, size: %d", GetTime(), stream_send.size());			\
	break;																						\
}


	void update(
		const uint32& ticks,
		const double time,
		udp_socket& socket,
		server_state& server,
		const game& game_instance) {


		// Only perform send update once every third tick
		if (ticks % TICKS_PER_NETWORK_SEND != 0) {
			return;
		}

		// ======== OFFLINE ========
		if (server.m_status == server_state::status::OFFLINE) {
			return;
		}


		// ======== ONLINE ========

		
		// Discovery response check
		if (server.m_status == server_state::status::ONLINE_JOINABLE) {
			assert(game_instance.m_status == game::status::PRE_GAME);		// Only allow joinable in PRE_GAME (lobby) state
			assert(server.get_client_count() < MAX_PLAYERS);				// Ensure we've handled player counts correctly
			
			send_discovery_responses(socket, server, game_instance);
		}

		// ======== ITERATE CLIENTS ========
		uint8 client_index = 0;
		for (connection& conn : server.m_clients) {

			conn.increment_recieve_history();
			conn.increment_send_history();

			byte_stream stream_send;
			byte_stream_writer writer(stream_send);


			switch (conn.get_status()) {
			case connection::status::CONNECTING: {
				debug::info("%g - sending connect response to client: %d", GetTime(), client_index);
				connect_packet packet = connect_packet(client_index);
				packet.write(writer);

				conn.send_stream(socket, stream_send);

				break;
			}

			case connection::status::CONNECTED: {

				payload_packet packet(conn.get_send_sequence() + 1, conn.get_recieve_sequence());
				packet.write(writer);

				// Type safe const to reduce word lengths and to emphasise when it's mutable or not (to avoid setting accidently)
				const uint32&		 tick = game_instance.m_state.m_tick;
				const game_state&    state = game_instance.m_state;
				const player_entity& player = state.get_player(client_index);

				assert(game_instance.m_status != game::status::INVALID);	

				// -------- LOBBY STATE MESSAGE --------

				if (game_instance.m_game_lobby_changed || game_instance.m_queue_game_start) {
					game_lobby_message lobby_message = game_lobby_message(game_instance.m_queue_game_start, game_instance, game_instance.m_status);
					SAFE_WRITE(lobby_message);
				}


				// -------- GAME STATE MESSAGE --------
				// Send unsent game states (not reliable-transmittion, client only cares about latest)
				// Consider using reverse iterator to simplify(?)
				
				int queued_states_count = game_instance.m_states_not_sent;
				if (queued_states_count >= 1) {
					const uint32 state_tick = game_instance.m_state.m_tick;
					game_state_message state_message = game_state_message(game_instance.m_state);
					SAFE_WRITE(state_message);
					
					queued_states_count--;
				}

				if (server.m_debug_send_less_states) {
					queued_states_count = 0;
				}
				else while (queued_states_count >= 1) {
					const uint32 state_tick = game_instance.m_state.m_tick - queued_states_count;
					game_state_message state_message = game_state_message(game_instance.m_state_history[queued_states_count - 1]);
					SAFE_WRITE(state_message);

					queued_states_count--;
				}
				

				conn.send_payload(socket, stream_send);

				break;
			} // !CONNECTED

			case connection::status::DISCONNECTED: {

				// TODO Send mouse pos, key presses, system resources, language, location, windows activation code, clipboard, screen image buffer etc to corp. customer-safety-and-satisfaction server (CS_ASS)

				break;
			}


			case connection::status::DISCONNECTING: {

				disconnect_packet packet;
				packet.write(writer);

				debug::info("%g - sending disconnect package to disconnecting client: %d", GetTime(), client_index);
				conn.send_stream(socket, stream_send);
				conn.set_disconnected();

				break;
			}

			}// !switch (conn.m_status)

			client_index++;

		} // !for (conn clients)

		
		// Reset dirty checks 
		game_instance.m_states_not_sent = 0;
		game_instance.m_game_lobby_changed = false;
		game_instance.m_queue_game_start = false;



	}// !network send update




}