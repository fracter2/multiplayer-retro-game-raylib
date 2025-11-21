// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <thread>
#include <chrono>
#include <cstdio>
#include <stdio.h>
#include <iostream>

//#include "raylib.h"
//#include "common.hpp"
//#include "messages.hpp"
//#include "input.hpp"
//#include "game.hpp"
//#include "protocol.hpp"
//#include "connection.hpp"
//#include "network.hpp"

#include "server_state.hpp"
#include "server_recieve_system.hpp"
#include "server_send_system.hpp"
#include "server_render_system.hpp"
#include "server_game_system.hpp"

//#define _SERVER   // Already defined as a "global" macro in server proj file

int main()
{
	static constexpr std::string_view window_title = "Bomberman SERVER";

	InitWindow(meteor::render::WINDOW_WIDTH, meteor::render::WINDOW_HEIGHT, window_title.data());
	SetExitKey(0);			// Esc

	using namespace meteor;

	// ==== APP DATA ====
	constexpr uint16 PORT = 54321;
	//const ip_endpoint LOCAL_ENDPOINT(ip_address(10, 12, 234, 103), PORT);	// TODO make use of pre-made local adress getter func
	ip_endpoint local_endpoint = {};
	udp_socket socket = {};


	//connection server_connection = {};
	//connection clients[MAX_PLAYERS] = {};
	server_state server = {};

	game			   game_instance = {};
	input::input_state input = {};
	ui::main_menu	   menu = {};

	double next_tick_time = GetTime();
	uint32 ticks = 0;
	float dt = GetFrameTime();
	double time = GetTime();
	bool running = true;

	Texture texture = LoadTexture("../meteor/data/tiles.png");
	// TODO assert texture is actually loaded
	
	// ==== INIT ====
	network::startup boot;
	setup_socket_endpoint(socket, local_endpoint, PORT);

	debug::info("local endpoint: %d.%d.%d.%d:%d",
		local_endpoint.m_address.a(),
		local_endpoint.m_address.b(),
		local_endpoint.m_address.c(),
		local_endpoint.m_address.d(),
		local_endpoint.port());


    //std::cout << "Hello World!\n";

	// update loop
	while (running) {
		dt = GetFrameTime();
		running &= !WindowShouldClose();
		time = GetTime();
		

		//client_recieve_system::update(time, socket, server_connection, game);
		server_recieve_system::update(time, server, socket, game_instance, local_endpoint);
		

		// tick loop
		if (time >= next_tick_time) {
			next_tick_time += TICK_TIME;
			ticks += 1;

			// Use struct for all inputs, like "input_map"
			// Then convert to input Action, like move_requests
			input::update(input);

			
			//game_update_system::update(game, input);
			server_game_system::update(ticks, dt, game_instance, input, server);

			server_send_system::update(ticks, time, socket, server, local_endpoint, game_instance);

			BeginDrawing();
			//render_system::render(ticks, time, game, server_connection, texture);
			render::server_system(ticks, time, game_instance, server, texture, menu);
			EndDrawing();

		} //!tick loop

		// note: save the forest!
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

	} //!update loop

	CloseWindow();
	return 0;
} // !server

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
