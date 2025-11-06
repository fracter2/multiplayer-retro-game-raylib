// main.cpp

#include <thread>
//#include <chrono>
#include <cstdio>
#include <stdio.h>

#include "common.hpp"
#include "timer.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "connection.hpp"
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
	const int window_width = 1280, window_height = 720;
	const std::string_view window_title = "Bomberman CLIENT";

	InitWindow(window_width, window_height, window_title.data());
	//InitAudioDevice();	// No audio (yet)
	SetExitKey(0);			// Esc

	using namespace meteor;


	// ==== APP DATA ====
	constexpr uint16 PORT = 54321;
	//const ip_endpoint LOCAL_ENDPOINT(ip_address(10, 12, 234, 103), PORT);	// TODO make use of pre-made local adress getter func
	const ip_endpoint SERVER_ENDPOINT(ip_address(192, 168, 1, 72), PORT);	// TODO Add way of inputting adresses after start...
	ip_endpoint local_endpoint = {};
	ip_endpoint server_endpoint = SERVER_ENDPOINT;
	udp_socket socket = {};
	connection server_connection = {};

	game game_instance = {};
	input::input_state input = {};
	ui::main_menu menu = {};
	
	double next_tick_time = GetTime();
	uint32 ticks = 0;
	float dt = GetFrameTime();
	double time = GetTime();
	bool running = true;

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

	server_connection.m_status = connection::status::CONNECTING;		// TODO Move to system or dedicated INIT
	//debug::info("attempting to connect by default...");


	
	

	// update loop
	while (running) {
		dt = GetFrameTime();
		running &= !WindowShouldClose();
		time = GetTime();
		


		client_recieve_system::update(time, socket, server_connection, game_instance);

		// tick loop
		if (time >= next_tick_time) {
			next_tick_time += TICK_TIME;

			input::update(input);

			game_update_system::update(game_instance, input);

			client_send_system::update(ticks, time, socket, server_connection, local_endpoint, server_endpoint, game_instance);

			BeginDrawing();
			render::client_system(ticks, time, game_instance, server_connection, texture, menu);
			EndDrawing();

		} //!tick loop

		// note: save the forest!
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

	} //!update loop

	CloseWindow();
	return 0;
}//!main


