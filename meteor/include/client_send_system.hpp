// client_send_system.hpp

#pragma once

#include "common.hpp"
#include "network.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "connection.hpp"
#include "game.hpp"

#include "input.hpp"

namespace meteor::client_send_system {

	constexpr double PHYS_TICKS_PER_NETWORK_TICK   = 3;	// 60hz / 3 = 20hz
	constexpr double CONNECTING_RESEND_DELAY_TICKS = 5; // 5 network-ticks

	

	/*
	// TODO Refrence connection status (const), game state (const)
	// TODO QUEUE SEND DATA, INPUT ACTIONS, WRAP IN TICK CLOSURE
	// note: network send update
	void update(const double time, udp_socket& socket, connection& connection, const ip_endpoint& client_endpoint, const ip_endpoint& server_endpoint, game::game& game_instance) {

		
		if (time > syncer.m_next_update_time) {
			syncer.m_last_checked_time = time;

			byte_stream stream_send;
			byte_stream_writer writer(stream_send);

			switch (serv_con.m_status) {
			case connection::status::CONNECTING: {
				debug::info("sending connect package");
				connect_packet message;
				message.write(writer);
				if (!socket.send_to(server_endpoint, stream_send)) { print_error_code(); }
				syncer.m_next_update_time = time + CONNECTING_DELTA;
				break;
			}

			case connection::status::CONNECTED: {
				//send_sequence += 1;
				payload_packet packet(game_instance.m_tick);	// TODO Sequence is just for recieve. SEND should use game tick!!
				packet.write(writer);

				mouse_position_message message((float)GetMouseX(), (float)GetMouseY());
				message.write(writer);

				latency_message ping_message(GetTime());
				ping_message.write(writer);

				// TODO MAKE THIS INTO A QUEUE FROM INPUT STATES CHECK
				// TODO GET INPUT IN SEPERATE SYSTEM BEFORE THIS
				// TODO SEND USERS CLIENT using PLAYER_ID 
				// TODO SEND USER INPUT IN DEDICATED INPUT MESSAGE

				// TODO Use input history singleton instead of quering
				//input::input_state input = input::get_current_input();
				movement_request move_request = movement_request::NEUTRAL;

				int x_axis = input.m_right - input.m_left;
				int y_axis = input.m_up - input.m_down;
				if		(x_axis == 1)	move_request = movement_request::RIGHT;
				else if (x_axis == -1)	move_request = movement_request::LEFT;
				else if (y_axis == 1)	move_request = movement_request::DOWN;
				else if (y_axis == -1)	move_request = movement_request::UP;

				entity_state_message state_message(game_instance.m_player_id, Vector2(0, 0), Color(0, 0, 0, 0), move_request);
				state_message.write(writer);

				if (!socket.send_to(server_endpoint, stream_send)) { print_error_code(); }
				debug::info("sending payload package");

				// note: timeout
				if (time > serv_con.m_last_recieve_time + TIMEOUT) {
					serv_con.m_status = connection::status::DISCONNECTED;
					debug::info("Timeout");
				}

				syncer.m_next_update_time = time + UPDATE_DELTA;

				break;
			}

			case connection::status::DISCONNECTED: {
				if (syncer.m_auto_connect) {
					serv_con.m_status = connection::status::CONNECTING;
				}
				game_instance = {};
				serv_con.m_sequence = 0;
				serv_con.m_acknowledge = 0;
				//send_sequence = 0;

				break;
			}


			case connection::status::DISCONNECTING: {
				serv_con.m_status = connection::status::DISCONNECTED;
				break;
			}

			}// !switch (server_connection.m_status)


		} // !network send update
	}
	*/
}
