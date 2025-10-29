// client_recieve_system.hpp

#pragma once

#include "common.hpp"
#include "network.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "connection.hpp"
#include "game.hpp"

#include "input.hpp"

namespace meteor::client_recieve_system {
	void update(double time, udp_socket& socket, connection& conn, game::game& game) {

		// TODO UPDATE GAME STATE AND LATENCY STATE 
		// (reconsile if mispredicted on specific tick, repeat non-acked INPUTS for client prediciton in latency state)
		// TODO Refrence connection state and game state

		// As client, update game state imidiately
		// Reply on send check
		// Queue to make sure we have game-states in a short buffer 


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

		// TODO Add switch / array for connection state function pointers, that will handle 
		if (p > (uint8)protocol_packet_type::PAYLOAD) {	// check if it's above max protocol uint8
			debug::info("%g ignoring - recieved unknown protocol.", GetTime());
			continue;
		}
		protocol_packet_type protocol = (protocol_packet_type)p;

		// Protocol switch
		switch (protocol) {
		case protocol_packet_type::CONNECT:
		{
			// Make packet
			connect_packet packet;
			if (!packet.read(reader))				  { debug::info("%g - error reading connect package", GetTime()); print_error_code(); break; }
			if (packet.m_version != PROTOCOL_VERSION) { debug::info("%g - recieved bad connect protocol version", GetTime()); break; }
			if (packet.m_magic != PROTOCOL_MAGIC)	  { debug::info("%g - recieved bad connect magic version", GetTime()); break; }
			conn.m_last_recieve_time = time;

			// Status switch	// TODO Consider delegating switch into inline func, for readability (protocol switch with nested status switch is confusing)
			switch (conn.m_status) {
			case connection::status::DISCONNECTED:
			{
				conn.m_endpoint = sender_endpoint;
				conn.m_status = connection::status::CONNECTING;
				debug::info("%g - recieved broadcast", GetTime());
				break;
			}
			case connection::status::CONNECTING:
			{
				conn.m_status = connection::status::CONNECTED;
				debug::info("%g - gracefully connected to server", GetTime());
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

		case protocol_packet_type::DISCONNECT:
		{
			disconnect_packet packet;
			if (!packet.read(reader)) { debug::info("%g - error reading disconnect package", GetTime()); print_error_code(); break; }
			conn.m_last_recieve_time = time;

			// Status switch	// TODO Consider delegating switch into inline func, for readability (protocol switch with nested status switch is confusing)
			switch (conn.m_status) {
			case connection::status::DISCONNECTING:
			{
				debug::info("%i - Gracefully disconnected", GetTime());
				break;
			}
			case connection::status::CONNECTING:
			{
				debug::info("%i - Denied connection", GetTime());
				break;
			}
			case connection::status::CONNECTED:
			{
				// note: I assume this does not need to be gracefull, as it is more rare and gracefullness wouldn't help the kicked / unkicked players
				debug::info("%i - Got kicked (server initiated disconnect. disgracefull)", GetTime());
				break;
			}

			default: 
			{
				debug::info("%g - recieved disconnect package when irrellevant", GetTime());
				break;
			}
			}//!Status switch

			
			conn.m_status = connection::status::DISCONNECTED;

			// TODO RESET GAME STATE or FREEZE AND SHOW DISCONECT POPUP

			break;
		}//!Disconnect


		// TODO DELEGATE TO SEPARATE GAME SYNCER FILE
		// TODO MIND TICK CLOSURES
		// TODO MIND LATENCY STATE and RECONCILIATION
		// TODO MIND SENDER SEQUENCE and ACK
		case protocol_packet_type::PAYLOAD:
		{
			payload_packet packet;
			if (!packet.read(reader)) { debug::info("%g - error reading payload package", GetTime()); print_error_code(); break; }
			conn.m_last_recieve_time = time;

			if (conn.m_status != connection::status::CONNECTED) {
				debug::info("%g - recieved payload package when irrelevant", GetTime());
				continue;
			}

			//game.m_time_sec = GetTime();

			if (packet.m_sequence <= conn.m_sequence) {
				debug::info("out-of-order packet dropped. my server sequenece: %d, packet sequence: %d, time: %f "
					, (conn.m_sequence)
					, (packet.m_sequence)
					, (GetTime())
				);
				break;
			}
			conn.m_sequence = packet.m_sequence;

			

			while (reader.has_data())
			{
				uint8 type = reader.peek();

				switch (type) {
				case (uint8)message_type::LATENCY: {
					latency_message message;
					if (!message.read(reader)) { print_error_code(); break; }
					debug::info("latency: %f ", (GetTime() - message.m_time));
					break;
				}

				case (uint8)message_type::ENTITY_STATE: {
					entity_state_message message;
					if (!message.read(reader)) { print_error_code(); break; }

					game_instance.update_entity(message);

					break;
				}
				} // !payload switch
			}

			break;
		}
		} // !protocol switch

	} // !while socket.has_data()
	}

}