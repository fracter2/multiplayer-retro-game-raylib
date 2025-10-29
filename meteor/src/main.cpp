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
#include "client_recieve_system.hpp"
#include "game_update_system.hpp"
#include "render_system.hpp"

#define _CLIENT // This is also added in project settings, making it "global"


int main(int argc, char **argv)
{
	const int window_width = 1280, window_height = 720;
	const std::string_view window_title = "Drawing over IP client";
	InitWindow(window_width, window_height, window_title.data());
	SetExitKey(0);		// Esc

	using namespace meteor;
	network::startup boot;

	
	const ip_endpoint LOCAL_ENDPOINT(ip_address(10, 12, 234, 103), 54321);	// TODO make use of pre-made local adress getter func
	const ip_endpoint SERVER_ENDPOINT(ip_address(10, 12, 190, 110), 54321);	// TODO Add way of inputting adresses after start...


	server_connection_syncer server_syncer;
	connection& serv_con = server_syncer.m_connection;

	server_syncer.m_connection.m_status = connection::status::CONNECTING;		// TODO Move to system or dedicated INIT
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
	//const double GAME_UPDATE_DELTA = 1 / 60;
	const bool auto_reconnect = true;


	game::game		   game	 = {};
	input::input_state input = {};

	// update loop
	bool running = true;
	while (running) {
		const float dt = GetFrameTime();
		running &= !WindowShouldClose();
		
		double time = GetTime();
		double next_tick_time = time;

		

		
		client_recieve_system::update(socket);

		// tick loop
		if (time > next_tick_time) {
			next_tick_time += TICK_TIME;

			// Use struct for all inputs, like "input_map"
			// Then convert to input Action, like move_requests
			input::update(input);
			// Then save alongside (inside?) game state buffer


			game_update_system::update(game, input);


			// TODO MOVE SEND CHECK HERE TO FILE, 20hz
			// Reply ACK with latest recieve... (done by sending client game tick? or both, in case of missed server-packages?)
			// Input and client-prediction is in latency state...
			// Input is sent ASAP (20hz), whenever the server recieves it, it uses it
			// INPUT IS TICK-WRAPPED ON THE CLIENT-TICK IT WAS PLAYED (Still would send latest input asap). Latest recieved game tick/package is also sent sepparately (ACK)

			client_send_system::update(time, socket, server_syncer, LOCAL_ENDPOINT, SERVER_ENDPOINT, game);



			BeginDrawing();
			render_system::render();
			EndDrawing();
		} //!tick loop

		// note: save the forest!
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	} //!update loop

	CloseWindow();
	return 0;
}//!main
