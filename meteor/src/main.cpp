// main.cpp

#include <thread>
#include <chrono>
#include <cstdio>
#include <stdio.h>

#include "common.hpp"
#include "timer.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "connection.hpp"
#include "game.hpp"

#include "input.hpp"
#include "client_send_system.hpp"

#define CLIENT

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
	SetExitKey(0);		// Esc

	using namespace meteor;
	network::startup boot;

	const ip_endpoint LOCAL_ENDPOINT(ip_address(10, 12, 234, 103), 54321);
	const ip_endpoint SERVER_ENDPOINT(ip_address(10, 12, 190, 110), 54321);

	server_connection_syncer server_syncer;
	connection& serv_con = server_syncer.m_connection;

	server_syncer.m_connection.m_status = connection::status::CONNECTING;
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

	// TODO Move all these to network state
	double prev_send_time = GetTime();
	double target_time = GetTime();				
	const double GAME_UPDATE_DELTA = 1 / 60;
	
	const bool auto_reconnect = true;


	// Game state data
	game game_instance = {};
	


	// update loop

	bool running = true;
	while (running) {
		const float dt = GetFrameTime();
		running &= !WindowShouldClose();
		
		double time = GetTime();
		// TODO USE THIS TIME EVERYWHERE as the CURRENT TICK TIME or similar. Util class?

		

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
				if (serv_con.m_status == connection::status::CONNECTING) {
					serv_con.m_status = connection::status::CONNECTED;
					serv_con.m_last_recieve_time = GetTime();
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
				serv_con.m_last_recieve_time = GetTime();

				if (serv_con.m_status == connection::status::DISCONNECTING)
					debug::info("%i - Gracefully disconnected", GetTime());
				else if (serv_con.m_status == connection::status::DISCONNECTED)
					debug::info("%i - recived disconnect package when already disconneced", GetTime());
				else
					debug::info("%i - Disgracefull disconnect", GetTime());

				serv_con.m_status = connection::status::DISCONNECTED;
				
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
				serv_con.m_last_recieve_time = GetTime();
				game_instance.m_time_sec = GetTime();

				if (packet.m_sequence <= serv_con.m_sequence) {
					debug::info("out-of-order packet dropped. my server sequenece: %d, packet sequence: %d, time: %f "
						, (serv_con.m_sequence)
						, (packet.m_sequence)
						, (GetTime())
					); 
					break; 
				}
				serv_con.m_sequence = packet.m_sequence;
				

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

						game_instance.update_entity(message);

						break;
					}
					} // !payload switch
				}

				break;
			}
			} // !protocol switch
			
		} // !while socket.has_data()


		// ---- OVERALL STRUCTURE PLANNING ----

		// RECIEVE PACKETS, every frame
		// As client, update game state imidiately
		// Reply on send check
		// Queue to make sure we have game-states in a short buffer 

		// TODO QUERY INPUT, 60hz
		// Use struct for all inputs, like "input_map"
		// Then convert to input Action, like move_requests
		input::input_state inputs = input::get_current_input();

		

		// Then save alongside (inside?) game state buffer


		// TODO GAME UPDATE LOOP, 60hz
		// USE GAME STATES
		// USE LATENCY STATE for CLIENT PREDICTION
		game_instance.update_process();

		// TODO MOVE SEND CHECK HERE TO FILE, 20hz
		// For both GAME STATE and CONNECTING
		// Reply ACK with latest recieve... (done by sending client game tick? or both, in case of missed server-packages?)
		// Input and client-prediction is in latency state...
		// Input is sent ASAP (20hz), whenever the server recieves it, it uses it
		// INPUT IS TICK-WRAPPED ON THE CLIENT-TICK IT WAS PLAYED (Still would send latest input asap). Latest recieved game tick/package is also sent sepparately (ACK)
		
		meteor::client_send_system::update(time, socket, server_syncer, LOCAL_ENDPOINT, SERVER_ENDPOINT, game_instance);
		//client_send_system(time);
		
		
		// TODO RENDER GAME, 60hz or vsync (would need lerp logic)
		BeginDrawing();
		game_instance.render_frame();

		//DrawRectangle(my_x, my_y, 10, 10, GREEN);
		//DrawRectangle(server_x, server_y, 10, 10, YELLOW);

		// TODO RENDER UI
		//DrawFPS(2, 2);

		EndDrawing();


		// END LOOP, sleep for 1 ms
		// note: save the forest!
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	CloseWindow();

	return 0;
}
