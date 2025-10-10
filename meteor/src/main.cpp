// main.cpp

#include <thread>
#include <chrono>
#include <cstdio>

#include <stdio.h>
#include "raylib.h"
#include "timer.hpp"
#include "network.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "connection.hpp"
#include "game.hpp"

static void
print_error_code()
{
	using namespace meteor;
	auto now = time::get_current_time_ms();
	auto error = network::get_last_error();
	debug::error("%3.2fs - %5d: %s",
		time::elapsed_seconds(now),
		error.code(),
		error.c_str());
}



int main(int argc, char **argv)
{
	const int window_width = 1280, window_height = 720;
	const std::string_view window_title = "Drawing over IP client";
	InitWindow(window_width, window_height, window_title.data());
	//SetTargetFPS(60);
	SetExitKey(0);		// Esc

	using namespace meteor;
	network::startup boot;

	const ip_endpoint LOCAL_ENDPOINT    (ip_address(10,12,234,103),  54321);
	const ip_endpoint SERVER_ENDPOINT   (ip_address(10, 12, 190, 110),   54321);

	connection server_connection;
	server_connection.m_status = connection::status::CONNECTING;
	debug::info("attempting to connect by default...");

	udp_socket socket;
	if (!socket.open_and_bind(LOCAL_ENDPOINT)) {
		print_error_code();
		return 0;
	}

	debug::info("local endpoint: %d.%d.%d.%d:%d",
		LOCAL_ENDPOINT.m_address.a(),
		LOCAL_ENDPOINT.m_address.b(),
		LOCAL_ENDPOINT.m_address.c(),
		LOCAL_ENDPOINT.m_address.d(),
		LOCAL_ENDPOINT.port());

	//int game_frame = 0;
	double prev_send_time = GetTime();
	double target_time = GetTime();
	const double GAME_UPDATE_DELTA = 1 / 60;
	const double SEND_UPDATE_DELTA = 1 / 20;
	const double CONNECTING_DELTA = 2;

	const bool auto_reconnect = true;


	// Game state data
	game m_game = {};
	


	// update loop

	bool running = true;
	while (running) {
		const float dt = GetFrameTime();
		running &= !WindowShouldClose();
		
		double time = GetTime();
		// TODO USE THIS TIME EVERYWHERE as the CURRENT TICK TIME or similar. Util class?

		// TODO MOVE TO FILE, 20hz
		// TODO Refrence connection status (const), game state (const)
		// TODO QUEUE SEND DATA, INPUT ACTIONS, WRAP IN TICK CLOSURE
		// note: network send update
		if (GetTime() > target_time) {
			prev_send_time = GetTime();

			byte_stream stream_send;
			byte_stream_writer writer(stream_send);

			switch (server_connection.m_status) {
			case connection::status::CONNECTING: {
				debug::info("sending connect package");
				connect_packet message;
				message.write(writer);
				if (!socket.send_to(SERVER_ENDPOINT, stream_send)) { print_error_code(); }
				target_time = GetTime() + CONNECTING_DELTA;
				break;
			}

			case connection::status::CONNECTED: {
				//send_sequence += 1;
				payload_packet packet(m_game.m_tick);	// TODO Sequence is just for recieve. SEND should user game tick!!
				packet.write(writer);

				mouse_position_message message((float)GetMouseX(), (float)GetMouseY());
				message.write(writer);

				latency_message ping_message(GetTime());
				ping_message.write(writer);

				// TODO MAKE THIS INTO A QUEUE FROM INPUT STATES CHECK
				// TODO GET INPUT IN SEPERATE SYSTEM BEFORE THIS
				// TODO SEND USERS CLIENT using PLAYER_ID 
				entity_state_message state_message(player_id, player_input, misc player stuff);
				state_message.write(writer);

				if (!socket.send_to(SERVER_ENDPOINT, stream_send)) { print_error_code(); }
				debug::info("sending payload package");

				// note: timeout
				if (GetTime() > server_connection.m_last_recieve_time + TIMEOUT) {
					server_connection.m_status = connection::status::DISCONNECTED;
					debug::info("Timeout");
				}

				target_time = GetTime() + SEND_UPDATE_DELTA;

				break;
			}

			case connection::status::DISCONNECTED: {
				if (auto_reconnect) { 
					server_connection.m_status = connection::status::CONNECTING; 
				}
				m_game = {};
				server_connection.m_sequence = 0;
				server_connection.m_acknowledge = 0;
				//send_sequence = 0;
				
				break;
			}
				

			case connection::status::DISCONNECTING: {
				server_connection.m_status = connection::status::DISCONNECTED;
				break;
			}

			}// !switch (server_connection.m_status)
			
			
		} // !network send update

		// TODO MOVE TO FILE, 60hz or more?
		// more to correct/reconsile next state ASAP
		// TODO UPDATE GAME STATE AND LATENCY STATE 
		// (reconsile if mispredicted on specific tick, repeat non-acked INPUTS for client prediciton in latency state)
		// TODO Refrence connection state and game state

		// note: recieve data
		while (socket.has_data()) {
			byte_stream stream_recieve;
			ip_endpoint sender_endpoint;
			if (!socket.receive_from(sender_endpoint, stream_recieve)) { 
				debug::info("socket recieve failed. aborting recieve.");
				break; 
			}
			if (sender_endpoint != SERVER_ENDPOINT) {
				debug::info("%g - !! recieving from NON-SERVER endpoint: %d.%d.%d.%d:%d, data size: %d",
					GetTime(),
					sender_endpoint.address().a(),
					sender_endpoint.address().b(),
					sender_endpoint.address().c(),
					sender_endpoint.address().d(),
					sender_endpoint.port(),
					stream_recieve.size());
				break;
			}

			byte_stream_reader reader(stream_recieve);
			debug::info("%g - recieving from server, data size: %d",
				GetTime(),
				stream_recieve.size());

			uint8 protocol = reader.peek();

			switch (protocol) {
			case (uint8)protocol_packet_type::CONNECT:
			{
				connect_packet packet;
				if (!packet.read(reader)) { print_error_code(); break; }
				if (server_connection.m_status == connection::status::CONNECTING) {
					server_connection.m_status = connection::status::CONNECTED;
					server_connection.m_last_recieve_time = GetTime();
					debug::info("%g - now connected to server", GetTime());
				}
				else {
					debug::info("%g - recieved connect package when irrellevant", GetTime());
				}
				break;
			}

			case (uint8)protocol_packet_type::DISCONNECT:
			{
				disconnect_packet packet;
				if (!packet.read(reader)) { print_error_code(); break; }
				server_connection.m_last_recieve_time = GetTime();

				if (server_connection.m_status == connection::status::DISCONNECTING)
					debug::info("%i - Gracefully disconnected", GetTime());
				else if (server_connection.m_status == connection::status::DISCONNECTED)
					debug::info("%i - recived disconnect package when already disconneced", GetTime());
				else
					debug::info("%i - Disgracefull disconnect", GetTime());

				server_connection.m_status = connection::status::DISCONNECTED;
				
				// TODO RESET GAME STATE or FREEZE AND SHOW DISCONECT POPUP

				break;
			}

			// TODO DELEGATE TO SEPARATE GAME SYNCER FILE
			// TODO MIND TICK CLOSURES
			// TODO MIND LATENCY STATE and RECONCILIATION
			// TODO MIND SENDER SEQUENCE and ACK
			case (uint8)protocol_packet_type::PAYLOAD:
			{
				payload_packet packet;
				if (!packet.read(reader)) { print_error_code(); break; }
				server_connection.m_last_recieve_time = GetTime();
				m_game.m_time_sec = GetTime();

				if (packet.m_sequence <= server_connection.m_sequence) { 
					debug::info("out-of-order packet dropped. my server sequenece: %d, packet sequence: %d, time: %f "
						, (server_connection.m_sequence)
						, (packet.m_sequence)
						, (GetTime())
					); 
					break; 
				}
				server_connection.m_sequence = packet.m_sequence;
				

				while(reader.has_data())
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

						m_game.update_entity(message);

						break;
					}
					} // !payload switch
				}

				break;
			}
			} // !protocol switch
			
		} // !while socket.has_data()

		// TODO QUERY INPUT

		// TODO GAME UPDATE LOOP, 60hz
		// TODO USE GAME STATES
		// TODO USE LATENCY STATE for CLIENT PREDICTION
		m_game.update_process();

		// TODO MOVE SEND CHECK HERE


		// TODO RENDER GAME, 60hz or vsync (need lerp logic

		// TODO RENDER UI

		// TODO END LOOP, sleep for 1 ms

		// DRAWING
		BeginDrawing();
		
		m_game.render_frame();

		// TODO DRAW ALL ENTITIES W COLOR
		//DrawRectangle(my_x, my_y, 10, 10, GREEN);
		//DrawRectangle(server_x, server_y, 10, 10, YELLOW);
		
		
		//DrawFPS(2, 2);
		EndDrawing();

		// note: save the forest!
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	CloseWindow();

	return 0;
}
