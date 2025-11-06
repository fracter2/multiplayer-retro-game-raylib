// server_send_system.cpp

#pragma once

#include "server_send_system.hpp"


namespace meteor::server_send_system {

	void disconnect_conn(connection& conn, server_state& server) {
		//conn.m_endpoint = {};				// We could let this stay until it is overriden by new player. This way we know who "recently deleted" 
											// and can reply with more disconnect packets, in case of packet loss.
		conn.m_status = connection::status::DISCONNECTED;
		conn.m_send_sequence = 0;
		conn.m_recieve_sequence = 0;
		conn.m_recieve_acknowledge = 0;
	}

	void send_broadcast(udp_socket& socket,
		server_state& server,
		const ip_endpoint& local_endpoint,
		const game& game_instance) {

		byte_stream stream_send;
		byte_stream_writer writer(stream_send);
		ip_endpoint broadcast_endpoint = ip_endpoint(network::get_broadcast_address(), local_endpoint.m_port);

		connect_packet packet = connect_packet((uint8)server.get_client_count());
		packet.write(writer);

		debug::info("%g - sending broadcast", GetTime());
		if (!socket.send_to(broadcast_endpoint, stream_send)) { print_error_code(); }
		else {
			// TODO LOG DATA SENT
		}
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

			if (server.m_broadcast) {
				send_broadcast(socket, server, local_endpoint, game_instance);
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

				if (!socket.send_to(conn.m_endpoint, stream_send)) { print_error_code(); }
				else {
					// TODO LOG DATA SENT
				}

				break;
			}

			case connection::status::CONNECTED: {
				conn.m_send_sequence += 1;
				payload_packet packet(conn.m_send_sequence, conn.m_recieve_sequence);
				packet.write(writer);


				// Type safe const to reduce word lengths and to emphasise when it's mutable or not (to avoid setting accidently)
				const uint32		 tick = game_instance.m_tick;
				const game_state&    state = game_instance.m_state;
				const player_entity& player = state.get_player(client_index);


				assert(game_instance.m_status != game::status::INVALID);	


				// -------- LOBBY STATE MESSAGE --------
				// TODO Send game lobby updates (If changed) including win/lose state

				game_lobby_message lobby_message = game_lobby_message();
				



				// -------- GAME STATE MESSAGE --------
				// Send unsent game states (not reliable-transmittion, client only cares about latest)
				// Consider using reverse iterator to simplify(?)
				const uint8 history_size = (uint8)game_instance.m_state_history.size();
				uint8 state_i = history_size - (game_instance.m_states_not_sent - 1);	// -1 so the for(state_i) to account for state history not having latest game.m_state

				for (uint8 i = state_i; i < history_size; i++) {
					const uint32 state_tick = game_instance.m_tick - game_instance.m_states_not_sent;	
					game_state_message message = game_state_message(game_instance.m_state_history[i], state_tick);

					if (writer.m_stream.can_fit(sizeof(message))) {	// LIMIT TO MAX PACKAGE SIZE
						message.write(writer);
						game_instance.m_states_not_sent -= 1;
					}
					else { break; }

				}

				if (game_instance.m_states_not_sent == 1) {
					game_state_message message = game_state_message(game_instance.m_state, game_instance.m_tick);
					if (writer.m_stream.can_fit(sizeof(message))) {	// LIMIT TO MAX PACKAGE SIZE
						message.write(writer);
						game_instance.m_states_not_sent -= 1;
					}
					else { break; }
				}

				debug::info("%g - sending payload package to client %d, size: %d", GetTime(), client_index, stream_send.size());
				if (!socket.send_to(conn.m_endpoint, stream_send)) { print_error_code(); }
				else {
					// TODO LOG DATA SENT
				}


				// -------- RELIABLE MESSAGES --------
				// TODO Send all un-aked messages for this client as RELIABLE MESSAGES using sequence wrapper (to allow client to avoid doubble-recieving a msg)

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
				if (!socket.send_to(conn.m_endpoint, stream_send)) { print_error_code(); }
				else {
					// TODO LOG DATA SENT

					disconnect_conn(conn, server);
				}

				break;
			}

			}// !switch (conn.m_status)

			client_index++;

		} // !for (conn clients)

		

		



	}// !network send update




}