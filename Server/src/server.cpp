// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <thread>
#include <chrono>
#include <cstdio>
#include <stdio.h>
#include <iostream>

#include "server_state.hpp"
#include "server_recieve_system.hpp"
#include "server_send_system.hpp"
#include "server_render_system.hpp"
#include "server_game_system.hpp"
#include "ui.hpp"

//#define _SERVER   // Already defined as a "global" macro in server proj file

int main()
{
	static constexpr std::string_view window_title = "Bomberman SERVER";

	InitWindow(meteor::render::WINDOW_WIDTH, meteor::render::WINDOW_HEIGHT, window_title.data());
	SetExitKey(0);			// Esc

	using namespace meteor;

	// ==== APP DATA ====
	constexpr uint16 PORT = 54321;
	ip_endpoint  local_endpoint = {};
	udp_socket   socket = {};
	server_state server = {};

	game		game_instance = {};
	input_state input = {};

	double next_tick_time = GetTime();
	uint32 ticks = 0;
	float  dt = GetFrameTime();
	double time = GetTime();
	bool   running = true;

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

	game_instance.init();
	

	// update loop
	while (running) {
		dt = GetFrameTime();
		running &= !WindowShouldClose();
		time = GetTime();
		

		//client_recieve_system::update(time, socket, server_connection, game);
		server_recieve_system::update(server, socket, game_instance, local_endpoint);
		

		// tick loop
		if (time >= next_tick_time) {
			next_tick_time += TICK_TIME;
			ticks += 1;

			input.update();

			ui::debug_skip_recieve_check(server, input);

			server_game_system::update(ticks, dt, game_instance, input, server);

			server_send_system::update(ticks, time, socket, server, local_endpoint, game_instance);

			BeginDrawing();
			render::server_system(ticks, game_instance, server, texture);
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

