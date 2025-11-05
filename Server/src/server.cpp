// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


//#include <thread>
//#include <chrono>
#include <cstdio>
#include <stdio.h>
#include <iostream>

//#include "raylib.h"
#include "common.hpp"
//#include "messages.hpp"
#include "input.hpp"
#include "game.hpp"
#include "protocol.hpp"
#include "connection.hpp"
#include "network.hpp"

//#define _SERVER   // Already defined as a "global" macro in server proj file

int main()
{
	const int window_width = 1280, window_height = 720;
	const std::string_view window_title = "Bomberman SERVER";

	InitWindow(window_width, window_height, window_title.data());
	SetExitKey(0);			// Esc

	using namespace meteor;

	// ==== APP DATA ====
	constexpr uint16 PORT = 54321;
	//const ip_endpoint LOCAL_ENDPOINT(ip_address(10, 12, 234, 103), PORT);	// TODO make use of pre-made local adress getter func
	const ip_endpoint SERVER_ENDPOINT(ip_address(192, 168, 1, 53), PORT);	// TODO Add way of inputting adresses after start...
	ip_endpoint local_endpoint = {};
	ip_endpoint server_endpoint = SERVER_ENDPOINT;
	udp_socket socket = {};
	connection server_connection = {};

	game::game		   game = {};
	input::input_state input = {};


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


    //std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
