// client_recieve_system.cpp

#pragma once

#include "client_recieve_system.hpp"
#include "protocol.hpp"
#include "messages.hpp"

namespace meteor::client_recieve_system {

	void update(double time, udp_socket& socket, connection& conn, game& game_instance) {

		// note: timeout
		if (conn.m_status != connection::status::DISCONNECTED
			&& time > conn.m_last_recieve_time + TIMEOUT) 
		{
			debug::info("Timeout");
			conn.set_disconnected();
		}


		while (socket.has_data()) {

			// Recieve
			byte_stream stream_recieve;
			ip_endpoint sender_endpoint;
			if (!socket.receive_from(sender_endpoint, stream_recieve)) {
				debug::info("socket recieve failed. aborting recieve.");
				return;	// skip entire method in case it gets resolved next loop
			}

			// Only accept non-server packages if in DISCONNECTED state
			if ((conn.m_status != connection::status::DISCONNECTED && sender_endpoint != conn.m_endpoint)) {
				debug::info("%g - !! ignoring - pck from NON-SERVER endpoint: %d.%d.%d.%d:%d, data size: %d",
					GetTime(),
					sender_endpoint.address().a(),
					sender_endpoint.address().b(),
					sender_endpoint.address().c(),
					sender_endpoint.address().d(),
					sender_endpoint.port(),
					stream_recieve.size());
				continue;	// skip to next in case theree are other packages
			}

			// Read stream
			byte_stream_reader reader(stream_recieve);
			//debug::info("%g - recieving, data size: %d", GetTime(), stream_recieve.size());

			uint8 p = reader.peek();
			if (p > (uint8)protocol_packet_type::PAYLOAD) {	// check if it's above max protocol uint8
				debug::info("%g ignoring - recieved unknown protocol.", GetTime());
				continue;
			}
			protocol_packet_type protocol = (protocol_packet_type)p;

			// Protocol switch
			switch (protocol) {

			// ======== CONNECT ========
			case protocol_packet_type::CONNECT:
			{
				connect_packet packet;
				if (!packet.read(reader)) { debug::info("%g - error reading connect package", GetTime()); print_error_code(); break; }
				if (packet.m_version != PROTOCOL_VERSION) { debug::info("%g - recieved bad connect protocol version", GetTime()); break; }
				if (packet.m_magic != PROTOCOL_MAGIC) { debug::info("%g - recieved bad connect magic version", GetTime()); break; }

				
				switch (conn.m_status) {
				case connection::status::DISCONNECTED:
				{
					conn = connection(sender_endpoint);
					conn.m_status = connection::status::CONNECTING;
					conn.m_last_recieve_time = time;
					debug::info("%g - recieved broadcast from server with %d player, attempting join", GetTime(), packet.m_player_id);
					debug::info("server endpoint: %d.%d.%d.%d:%d",
						sender_endpoint.m_address.a(),
						sender_endpoint.m_address.b(),
						sender_endpoint.m_address.c(),
						sender_endpoint.m_address.d(),
						sender_endpoint.port());
					break;
				}
				case connection::status::CONNECTING:
				{
					if (packet.m_broadcast) { break; }
					conn.m_status = connection::status::CONNECTED;
					debug::info("%g - gracefully connected to server as player %d", GetTime(), packet.m_player_id);

					game_instance = game();		// Reset game. note that we aren't allocating with "new", so no memory leaks.
					game_instance.m_user_index = packet.m_player_id;
					game_instance.m_status = game::status::PRE_GAME;

					break;
				}
				default:
				{
					debug::info("%g - recieved connect package when irrellevant", GetTime());
					break;
				}
				}//!Status switch

				break;
			}//!CONNECT


			// ======== DISCONNECT ========
			case protocol_packet_type::DISCONNECT:
			{
				disconnect_packet packet;
				if (!packet.read(reader)) { debug::info("%g - error reading disconnect package", GetTime()); print_error_code(); break; }

				switch (conn.m_status) {
				case connection::status::DISCONNECTING:
				{
					debug::info("%g - Gracefully disconnected", GetTime());
					break;
				}
				case connection::status::CONNECTING:
				{
					debug::info("%g - Denied connection", GetTime());
					break;
				}
				case connection::status::CONNECTED:
				{
					// note: I assume this does not need to be gracefull, as it is more rare and gracefullness wouldn't help the kicked / unkicked players
					debug::info("%g - Got kicked (server initiated disconnect)", GetTime());
					break;
				}

				default:
				{
					debug::info("%g - recieved disconnect package when irrellevant", GetTime());
					break;
				}
				}//!Status switch

				conn.m_last_recieve_time = time;
				conn.set_disconnected();
				
				break;
			}//!Disconnect


			// ======== PAYLOAD ========
			case protocol_packet_type::PAYLOAD:
			{
				const int packet_size = stream_recieve.size();
				payload_packet packet;
				if (!packet.read(reader)) { debug::info("%g - error reading payload package", GetTime()); print_error_code(); break; }
				
				if (!conn.can_recieve(packet)) continue;
				else conn.log_payload(packet, packet_size);

				uint32 msg_sequence = packet.m_sequence; // TODO FOR RELIABLE MESSAGES, set by message_type::SEQUENCE_WRAP, ignore message if conn.sequence is higher.
				

				// ---- MESSAGES ----
				while (reader.has_data())
				{
					uint8 t = reader.peek();
					if (t >= (uint8)message_type::MAX) {
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

						const uint32 msg_tick = message.m_game_state.m_tick;
						int ticks_ahead = (int)msg_tick - (int)game_instance.m_state.m_tick;

						if (ticks_ahead < 0) { debug::info("%g - recieved gamestate that is behind at tick: %d, local tick: %d, ignoring", GetTime(), msg_tick, game_instance.m_state.m_tick);
							break; 
						}
						if (ticks_ahead == 0) { debug::info("%g - recieved gamestate that is on current tick: %d", GetTime(), msg_tick);
							// TODO Consider replacing current with recieved new
							break; 
						}

						//if (ticks_ahead < game_instance.m_state_queue.size()) { debug::info("%g - recieved state for upcoming tick inbetween newest and current", GetTime()); }
						if (ticks_ahead == game_instance.m_state_queue.size()) { debug::info("%g - recieved state for already queued, newest tick", GetTime()); }
						
						while (ticks_ahead > game_instance.m_state_queue.size()) {
							game_instance.m_state_queue.push_back(game_state());
						}
						
						if (!game_instance.m_state_queue[ticks_ahead - 1].is_default()) {	// -1 because index '0' is 1 tick ahead (it's the *next* ticks)
							debug::info("overriding already queued non-default");
						}

						game_instance.m_state_queue[ticks_ahead - 1] = message.m_game_state;
						//debug::info("%g - recieved gamestate tick: %d, local tick: %d", GetTime(), message.m_tick, game_instance.m_tick);

						break;
					}
					// ---- INPUT_ACTION ----
					case message_type::INPUT_ACTION:
					{
						debug::info("%g - recieved input message as client... ignoring", GetTime());
						input_action_message message;	
						if (!message.read(reader)) { print_error_code(); break; }	// Read it so it gets consumed
						
						break;
					}
					// ---- GAME_LOBBY ----
					case message_type::GAME_LOBBY:
					{
						debug::info("%g - recieved game lobby state", GetTime());
						game_lobby_message message;
						if (!message.read(reader)) { print_error_code(); break; }

						// TODO Apply all game lobby info recieved
						for (int i = 0; i < MAX_PLAYERS; i++) {
							game_instance.m_player_info[i] = message.m_player_info[i];
						}

						if (message.m_start_now && game_instance.m_status == game::status::PRE_GAME) {
							game_instance.m_status = game::status::IN_GAME;
							debug::info("%g - Set game status 'IN_GAME'", GetTime());
						}
						break;
					}

					} // !messages switch
				} // !messages loop

				break;
			} // !payload case

			} // !protocol switch
		} // !while socket.has_data()
	} // !update()


}//!namespace

