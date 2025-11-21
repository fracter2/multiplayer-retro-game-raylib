// server_recieve_system.cpp

#pragma once

#include "server_recieve_system.hpp"

#include "common.hpp"
#include "protocol.hpp"
#include "messages.hpp"
#include "game.hpp"



namespace meteor::server_recieve_system {


	void update(const double time, server_state& server, udp_socket& socket, game& game_instance, ip_endpoint& local_endpoint) {

		{// timeout
			uint8 i = 0;
			for (connection& conn : server.m_clients) {
				if (conn.m_status != connection::status::DISCONNECTED
				&& time > conn.m_last_recieve_time + TIMEOUT)
				{
					debug::info("Timeout player: %d", i);
					conn.m_status = connection::status::DISCONNECTED;
					conn.m_send_sequence = 0;
					conn.m_recieve_sequence = 0;
					conn.m_recieve_acknowledge = 0;

					game_instance.m_player_info[i].m_player_status = player_info::status::TIMED_OUT;
					game_instance.game_lobby_changed = true;			// Queue for the send system to send lobby state message
				}
				i++;
			}
		} // !timeout


		while (socket.has_data()) {

			byte_stream stream_recieve;
			ip_endpoint sender_endpoint;
			if (!socket.receive_from(sender_endpoint, stream_recieve)) {
				debug::info("socket recieve failed. aborting recieve.");
				print_error_code();
				return;	// skip entire method in case it gets resolved next loop
			}

			if (sender_endpoint == local_endpoint) { continue; }

			// ======== OFFLINE ========
			if (server.m_status == server_state::status::OFFLINE) {
				debug::info("%g - !! ignoring pck as server is OFFLINE endpoint: %d.%d.%d.%d:%d, data size: %d",
					GetTime(),
					sender_endpoint.address().a(),
					sender_endpoint.address().b(),
					sender_endpoint.address().c(),
					sender_endpoint.address().d(),
					sender_endpoint.port(),
					stream_recieve.size());
				continue;	// skip to next packet in case there are more
			}


			// ======== ONLINE ========
			
			// Read stream
			byte_stream_reader reader(stream_recieve);
			//debug::info("%g - recieving, data size: %d", GetTime(), stream_recieve.size());
			
			if (reader.peek() >= (uint8)protocol_packet_type::MAX) {	// check if it's above max protocol uint8
				debug::info("%g ignoring - recieved unknown protocol: %f", GetTime(), reader.peek());
				continue;
			}
			protocol_packet_type protocol = (protocol_packet_type)reader.peek();


			// Protocol switch
			switch (protocol) {

				// ======== CONNECT ========
			case protocol_packet_type::CONNECT:
			{
				connect_packet packet;
				if (!packet.read(reader)) { debug::info("%g - error reading connect package", GetTime()); print_error_code(); break; }
				if (packet.m_version != PROTOCOL_VERSION) { debug::info("%g - recieved bad connect protocol version", GetTime()); break; }
				if (packet.m_magic != PROTOCOL_MAGIC) { debug::info("%g - recieved bad connect magic version", GetTime()); break; }

				if (!server.has_client(sender_endpoint)) {
					if (server.m_status == server_state::status::ONLINE_JOINABLE) {
						join_lobby(time, server, socket, game_instance, reader, sender_endpoint, packet);
						break;
					}
					// Reply with DISCONNECT packet if in pre-game phase or recently left, to get them to realize theyre disconnected (may help if there's packet loss)
					else if (game_instance.m_status == game::status::PRE_GAME || server.has_client_recently_left(sender_endpoint)) {
						// TODO Use server(?) to queue and then send messages
						// Otherwise this is still fine, they'll eventually timeout
						break;
					}
					else {
						debug::info("%g - ignoring packet from unknown sender", GetTime());
						debug::info("sender endpoint: %d.%d.%d.%d:%d",
							sender_endpoint.m_address.a(),
							sender_endpoint.m_address.b(),
							sender_endpoint.m_address.c(),
							sender_endpoint.m_address.d(),
							sender_endpoint.port());
						break;
					}
				}
				else { 
					debug::info("%g - ignoring irrelevant connect packet from client", GetTime()); 
					break; 
				}

			}//!CONNECT


			// ======== DISCONNECT ========
			case protocol_packet_type::DISCONNECT:
			{
				disconnect_packet packet;
				if (!packet.read(reader)) { debug::info("%g - error reading disconnect package", GetTime()); print_error_code(); break; }

				uint32 r_index = 0;
				uint8 client_index = 0;
				if (!server.has_client(sender_endpoint, r_index)) {
					debug::info("%g - ignoring irrelevant disconnect packet from unknown sender", GetTime());
					debug::info("sender endpoint: %d.%d.%d.%d:%d",
						sender_endpoint.m_address.a(),
						sender_endpoint.m_address.b(),
						sender_endpoint.m_address.c(),
						sender_endpoint.m_address.d(),
						sender_endpoint.port());
					break;
				}
				else {
					client_index = (uint8)r_index;
					assert((uint32)client_index == r_index);	// Check incase we have more than uint8 limit clients... *just in case someone increases it above this* (i know)
				}


				debug::info("%g - Gracefully Disconnecting client %d", GetTime(), client_index);
				connection& conn = server.m_clients[client_index];
				conn.m_status = connection::status::DISCONNECTING;	// Queue for the send system to send disconnect package
				conn.m_last_recieve_time = time;

				player_info::status leave_reason = player_info::status::USER_LEFT;
				if (game_instance.m_player_info[client_index].m_player_status == player_info::status::LOSER) 
					leave_reason = player_info::status::RAGEQUIT;

				game_instance.m_player_info[client_index].m_player_status = leave_reason;
				game_instance.game_lobby_changed = true;			// Queue for the send system to send lobby state message

				break;
			}//!Disconnect


			// ======== PAYLOAD ========
			case protocol_packet_type::PAYLOAD:
			{
				payload_packet packet;
				if (!packet.read(reader)) { debug::info("%g - error reading payload package", GetTime()); print_error_code(); break; }

				uint32 r_index = 0;
				uint8 client_index = 0;
				if (!server.has_client(sender_endpoint, r_index)) {
					debug::info("%g - ignoring irrelevant payload packet from unknown sender", GetTime());
					debug::info("sender endpoint: %d.%d.%d.%d:%d",
						sender_endpoint.m_address.a(),
						sender_endpoint.m_address.b(),
						sender_endpoint.m_address.c(),
						sender_endpoint.m_address.d(),
						sender_endpoint.port());
					
					// TODO CONSIDER CHECKING IF RECENTLY LEFT and send another disconnect if so, incase of packet loss

					break;
				}
				else {
					client_index = (uint8)r_index;
					assert((uint32)client_index == r_index);	// Check incase we have more than uint8 limit clients... *just in case*
				}
				

				//debug::info("%g - recieving payload from client %d", GetTime(), client_index);
				connection& conn = server.m_clients[client_index];
				
				// Conn status check
				if (conn.m_status == connection::status::CONNECTING) {
					conn.m_status = connection::status::CONNECTED;
					game_instance.m_player_info[client_index].m_player_status = player_info::status::ACTIVE;
					game_instance.game_lobby_changed = true;			// Queue for the send system to send lobby state message
					debug::info("%g - client %d joined gracefully", GetTime(), client_index);
				}
				if (conn.m_status != connection::status::CONNECTED) {
					
					debug::info("%g - recieved payload package when irrelevant", GetTime());
					continue;
				}

				// Sequence
				if (packet.m_sequence <= conn.m_recieve_sequence) {
					debug::info("out-of-order or duplicate packet dropped. my recieve_sequenece: %d, packet sequence: %d, time: %f "
						, (conn.m_recieve_sequence)
						, (packet.m_sequence)
						, (GetTime())
					);
					continue;
				}

				// move to after validation of good message type
				conn.m_last_recieve_time = time;
				conn.m_recieve_sequence = packet.m_sequence;
				conn.m_recieve_acknowledge = packet.m_acknowledge;
				

				// ---- MESSAGES ----
				while (reader.has_data())
				{
					uint8 t = reader.peek();
					if (t > (uint8)message_type::GAME_LOBBY) {	// check if it's above max message_type value
						debug::info("%g - recieved unknown message type.", GetTime());
						continue;
					}

					message_type type = (message_type)t;

					switch (type) {
						// ---- GAME_STATE ----
					case message_type::GAME_STATE:
					{
						game_state_message message;	
						if (!message.read(reader)) { print_error_code(); break; }

						debug::info("%g - recieved game state message as server... irrellevant", GetTime());
						//continue;

						break;
					}
					// ---- INPUT_ACTION ----
					case message_type::INPUT_ACTION:
					{
						//debug::info("%g - recieved input message from client %d", GetTime(), client_index);
						input_action_message message;
						if (!message.read(reader)) { print_error_code(); break; }

						game_instance.m_player_action_queue[client_index].append_new(message.m_action, message.m_tick);
						debug::info("%g - recieved input from player[%d] with input %d on tick %d", GetTime(), client_index, (uint8)message.m_action, message.m_tick);

						//game_instance.m_state.m_players[client_index].m_prev_action = message.m_action;
						//game_instance.m_state.m_players[client_index].m_prev_action_tick = message.m_tick;

						break;
					}
					// ---- GAME_LOBBY ----
					case message_type::GAME_LOBBY:
					{
						game_lobby_message message;
						if (!message.read(reader)) { print_error_code(); break; }
						debug::info("%g - recieved game lobby state as server... irrelevant", GetTime());
						//continue;
						
						break;
					}

					} // !messages switch
				} // !messages loop

				break;
			} // !payload case

			} // !protocol switch
		} // !while socket.has_data()


	} // !update()


	void join_lobby(const double& time, server_state& server, udp_socket& socket, game& game_instance, byte_stream_reader& reader, ip_endpoint sender_endpoint, connect_packet packet) {

		// TODO Insert sender IP and set conn to "Connecting". Set to "Connected" when recieving first payload
		uint8 i = 0;
		for (connection& conn : server.m_clients) {
			if (conn.m_status == connection::status::DISCONNECTED) {
				conn.m_status = connection::status::CONNECTING;
				conn.m_endpoint = sender_endpoint;
				conn.m_last_recieve_time = time;
				
				game_instance.m_player_info[i].m_player_status = player_info::status::JOINING;
				game_instance.game_lobby_changed = true;			// Queue for the send system to send lobby state message
				
				break;
			}
			i++;
		}
		debug::info("%g - client %d is joining", GetTime(), i);
		debug::info("client endpoint: %d.%d.%d.%d:%d",
			sender_endpoint.m_address.a(),
			sender_endpoint.m_address.b(),
			sender_endpoint.m_address.c(),
			sender_endpoint.m_address.d(),
			sender_endpoint.port());
	}

}