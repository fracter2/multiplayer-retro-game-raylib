// main.cpp

#include <thread>
#include <cstdio>
#include <stdio.h>

#include "client_send_system.hpp"
#include "client_recieve_system.hpp"
#include "game_update_system.hpp"
#include "render_system.hpp"
#include "ui.hpp"
#include "lag_optimizer.hpp"
#include "server_browser.hpp"

//#define _CLIENT // This is added in project settings, making it "global"


int main(int argc, char **argv)
{
	static constexpr std::string_view window_title = "Bomberman CLIENT";

	InitWindow(meteor::render::WINDOW_WIDTH, meteor::render::WINDOW_HEIGHT, window_title.data());
	SetExitKey(0);			// Esc

	using namespace meteor;


	// ==== APP DATA ====
	udp_socket  socket = {};
	connection  server_connection = {};
	game		game_instance = {};
	input_state input = {};
	server_browser browser = {};
	
	double next_tick_time = GetTime();
	uint32 ticks = 0;
	float  dt = GetFrameTime();
	double time = GetTime();
	bool   running = true;

	Texture texture = LoadTexture("data/tiles.png");


	// ==== INIT ====
	network::startup boot;
	setup_socket_no_endpoint(socket);


	// ==== UPDATE LOOP ====
	while (running) {
		dt = GetFrameTime();
		running &= !WindowShouldClose();
		time = GetTime();

		client_recieve_system::update(next_tick_time, socket, server_connection, game_instance, browser);

		// tick loop
		if (time >= next_tick_time) {
			next_tick_time += TICK_TIME;
			ticks += 1;

			lag_optimizer(next_tick_time, game_instance);

			input.update();

			ui::quit_check(server_connection, input);
			ui::debug_skip_recieve_check(server_connection, input);
			ui::delay_tick(next_tick_time, input);
			ui::quicken_tick(next_tick_time, input);
			ui::discover_servers(input, browser, game_instance);
			ui::update_server_browser(input, browser, server_connection);

			game_update_system::update(game_instance, input);

			client_send_system::update(ticks, socket, server_connection, game_instance, browser);


			BeginDrawing();
			render::client_system(ticks, game_instance, server_connection, texture, browser);
			EndDrawing();

		} //!tick loop

		// note: save the forest!
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

	} //!update loop

	CloseWindow();
	return 0;
}//!main


