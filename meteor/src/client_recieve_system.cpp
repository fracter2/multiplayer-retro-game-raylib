// client_recieve_system.cpp

#pragma once

#include "client_recieve_system.hpp"
#include "protocol.hpp"
#include "messages.hpp"

namespace meteor::client_recieve_system {

	void update(double time, udp_socket& socket, connection& conn, game::game& game) {
		// TODO UPDATE GAME STATE AND LATENCY STATE 
		// (reconsile if mispredicted on specific tick, repeat non-acked INPUTS for client prediciton in latency state)
		// TODO Refrence connection state and game state

		// As client, update game state imidiately
		// Reply on send check
		// Queue to make sure we have game-states in a short buffer 

		// note: timeout
		if (conn.m_status != connection::status::DISCONNECTED
			&& time > conn.m_last_recieve_time + TIMEOUT) 
		{
			debug::info("Timeout");
			disconnect(conn);
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
			if (conn.m_status != connection::status::DISCONNECTED && sender_endpoint != conn.m_endpoint) {
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
			debug::info("%g - recieving, data size: %d",
				GetTime(),
				stream_recieve.size());

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
					debug::info("%g - recieved broadcast from server with %f player, attempting join", GetTime(), packet.m_player_id);
					break;
				}
				case connection::status::CONNECTING:
				{
					conn.m_status = connection::status::CONNECTED;
					debug::info("%g - gracefully connected to server as player %f", GetTime(), packet.m_player_id);

					game = game::game();		// Reset game. note that we aren't allocating with "new", so no memory leaks.
					game.m_user_index = packet.m_player_id;
					game.m_status = game::game::status::PRE_GAME;

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
					debug::info("%g - Got kicked (server initiated disconnect. disgracefull)", GetTime());
					break;
				}

				default:
				{
					debug::info("%g - recieved disconnect package when irrellevant", GetTime());
					break;
				}
				}//!Status switch

				conn.m_last_recieve_time = time;
				disconnect(conn);
				
				break;
			}//!Disconnect


			// ======== PAYLOAD ========
			case protocol_packet_type::PAYLOAD:
			{
				payload_packet packet;
				if (!packet.read(reader)) { debug::info("%g - error reading payload package", GetTime()); print_error_code(); break; }
				

				if (conn.m_status != connection::status::CONNECTED) {
					debug::info("%g - recieved payload package when irrelevant", GetTime());
					continue;
				}

				if (packet.m_sequence <= conn.m_recieve_sequence) {
					debug::info("out-of-order packet dropped. my recieve_sequenece: %d, packet sequence: %d, time: %f "
						, (conn.m_recieve_sequence)
						, (packet.m_sequence)
						, (GetTime())
					);
					continue;
				}
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
						game_state_message message;		// WHY ERROR
						if (!message.read(reader)) { print_error_code(); break; }

						int ticks_ahead = message.m_tick - game.m_tick;

						if (ticks_ahead < 0) {
							debug::info("%g - recieved gamestate that is behind at tick: %d, local tick: %d, ignoring", GetTime(), message.m_tick, game.m_tick);
							break;
						}

						if (ticks_ahead == 0)						  debug::info("%g - recieved gamestate that is on current tick: %d", GetTime(), message.m_tick);
						else if (ticks_ahead < game.m_state_queue.size())  debug::info("%g - recieved state for upcoming tick inbetween newest and current", GetTime());
						else if (ticks_ahead == game.m_state_queue.size()) debug::info("%g - recieved state for already queued, newest tick", GetTime());
						//else // ticks_ahead > game.m_queued_states

						if (game.m_state_queue[ticks_ahead - 1].is_default())	// .is_default() means that tick is empty
							debug::info("overriding already queued tick");

						game.m_state_queue[ticks_ahead - 1] = message.m_game_state;

						break;
					}
					// ---- GAME_STATE ----
					case message_type::INPUT_ACTION:
					{
						debug::info("%g - recieved input message as client... ignoring", GetTime());
						break;
					}

					} // !messages switch
				} // !messages loop

				break;
			} // !payload case

			} // !protocol switch
		} // !while socket.has_data()
	} // !update()


	void disconnect(connection& conn) {
		conn.m_endpoint = {};
		conn.m_status = connection::status::DISCONNECTED;
		conn.m_send_sequence = 0;
		conn.m_recieve_sequence = 0;
		conn.m_recieve_acknowledge = 0;
	}

}//!namespace

