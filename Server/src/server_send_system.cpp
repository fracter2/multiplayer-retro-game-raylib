// server_send_system.cpp

#pragma once

#include "server_send_system.hpp"


namespace meteor::server_send_system {

	static void send_broadcast(udp_socket& socket,
		server_state& server,
		const ip_endpoint& local_endpoint,
		const game& game_instance) {

		byte_stream stream_send;
		byte_stream_writer writer(stream_send);
		ip_endpoint broadcast_endpoint = ip_endpoint(network::get_broadcast_address(), local_endpoint.m_port);

		connect_packet packet = connect_packet((uint8)server.get_client_count(), true);
		packet.write(writer);

		debug::info("%g - sending broadcast", GetTime());
		if (!socket.send_to(broadcast_endpoint, stream_send)) { print_error_code(); }
		else {
			// TODO LOG DATA SENT
		}
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
		const ip_endpoint& local_endpoint,
		const game& game_instance) {


		// Only perform send update once every third tick
		if (ticks % TICKS_PER_UPDATE != 0) {
			return;
		}

		// ======== OFFLINE ========
		if (server.m_status == server_state::status::OFFLINE) {
			return;
		}


		// ======== ONLINE ========

		
		// Broadcast check
		if (server.m_status == server_state::status::ONLINE_JOINABLE) {
			assert(game_instance.m_status == game::status::PRE_GAME);		// Only allow joinable in PRE_GAME (lobby) state
			assert(server.get_client_count() < MAX_PLAYERS);				// Ensure we've handled player counts correctly
			
			if (server.m_broadcast && server.m_next_broadcast_tick <= ticks) {
				send_broadcast(socket, server, local_endpoint, game_instance);
				server.m_next_broadcast_tick = ticks + server_state::BROADCAST_IDLE_TICKS;
				
			}
		}

		// ======== ITERATE CLIENTS ========
		uint8 client_index = 0;
		for (connection& conn : server.m_clients) {

			byte_stream stream_send;
			byte_stream_writer writer(stream_send);


			switch (conn.m_status) {
			case connection::status::CONNECTING: {
				debug::info("%g - sending connect response to client: %d", GetTime(), client_index);
				connect_packet packet;
				packet.write(writer);

				conn.send_stream(socket, stream_send);

				break;
			}

			case connection::status::CONNECTED: {

				payload_packet packet(conn.get_send_sequence(), conn.get_recieve_sequence());
				packet.write(writer);

				// Type safe const to reduce word lengths and to emphasise when it's mutable or not (to avoid setting accidently)
				const uint32&		 tick = game_instance.m_state.m_tick;
				const game_state&    state = game_instance.m_state;
				const player_entity& player = state.get_player(client_index);

				assert(game_instance.m_status != game::status::INVALID);	

				// -------- LOBBY STATE MESSAGE --------

				if (game_instance.game_lobby_changed || game_instance.queue_game_start) {
					game_lobby_message lobby_message = game_lobby_message(game_instance.queue_game_start, game_instance, game_instance.m_status);
					SAFE_WRITE(lobby_message);
				}


				// -------- GAME STATE MESSAGE --------
				// Send unsent game states (not reliable-transmittion, client only cares about latest)
				// Consider using reverse iterator to simplify(?)
				
				int queued_states_count = game_instance.m_states_not_sent;						// Make local for this conn-loop. Reset at end of update()
				if (queued_states_count >= 1) {
					const uint32 state_tick = game_instance.m_state.m_tick;
					game_state_message state_message = game_state_message(game_instance.m_state);
					SAFE_WRITE(state_message);
					
					queued_states_count--;
				}
				
				while (queued_states_count >= 1) {
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
		game_instance.game_lobby_changed = false;
		



	}// !network send update




}