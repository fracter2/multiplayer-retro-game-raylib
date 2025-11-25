// main.cpp

#include <thread>
//#include <chrono>
#include <cstdio>
#include <stdio.h>

//#include "common.hpp"
//#include "timer.hpp"
//#include "messages.hpp"
//#include "protocol.hpp"
//#include "connection.hpp"
//#include "game.hpp"

//#include "input.hpp"
#include "client_send_system.hpp"
#include "client_recieve_system.hpp"
#include "game_update_system.hpp"
#include "render_system.hpp"
#include "ui.hpp"


//#define _CLIENT // This is also added in project settings, making it "global"


int main(int argc, char **argv)
{
	static constexpr std::string_view window_title = "Bomberman CLIENT";

	InitWindow(meteor::render::WINDOW_WIDTH, meteor::render::WINDOW_HEIGHT, window_title.data());
	//InitAudioDevice();	// No audio (yet)
	SetExitKey(0);			// Esc

	using namespace meteor;


	// ==== APP DATA ====
	constexpr uint16 PORT = 54321;
	ip_endpoint local_endpoint = {};
	udp_socket  socket = {};
	connection  server_connection = {};

	game			   game_instance = {};
	input_state input = {};
	
	double next_tick_time = GetTime();
	uint32 ticks = 0;
	float  dt = GetFrameTime();
	double time = GetTime();
	bool   running = true;

	Texture texture = LoadTexture("data/tiles.png");

	// ==== INIT ====
	network::startup boot;
	setup_socket_endpoint(socket, local_endpoint, PORT);

	debug::info("local endpoint: %d.%d.%d.%d:%d",
		local_endpoint.m_address.a(),
		local_endpoint.m_address.b(),
		local_endpoint.m_address.c(),
		local_endpoint.m_address.d(),
		local_endpoint.port());

	// update loop
	while (running) {
		dt = GetFrameTime();
		running &= !WindowShouldClose();
		time = GetTime();
		
		// ---- TODO BEFORE THIS IS DONE ----
		// TODO APPLY INTERPOLATION FLAG on game-states AND SHOW INTERPOLATION FROM-TOO ON CLIENT
		// TODO FIX CLIENT-PREDICTION TWERKING (is it because interpolated-states dont increment input tick?)

		// TODO ADD SUMMARY OF BYTES PER SECOND AND MAKE SEND/RECIEVE ACCUMULATE & LOGG EVER TICK (instead of per-packet)
		// TODO WRITE ABOUT...


		client_recieve_system::update(next_tick_time, socket, server_connection, game_instance);

		// tick loop
		if (time >= next_tick_time) {
			next_tick_time += TICK_TIME;
			ticks += 1;

			input.update();

			ui::quit_check(server_connection, input);
			ui::debug_skip_recieve_check(server_connection, input);

			game_update_system::update(game_instance, input);

			client_send_system::update(ticks, socket, server_connection, local_endpoint, game_instance);

			BeginDrawing();
			render::client_system(ticks, game_instance, server_connection, texture);
			EndDrawing();

		} //!tick loop

		// note: save the forest!
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

	} //!update loop

	CloseWindow();
	return 0;
}//!main


