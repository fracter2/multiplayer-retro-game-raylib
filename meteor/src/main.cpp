// main.cpp

#include <stdio.h>
#include <thread>
#include <chrono>
#include "network.hpp"
#include "messages.hpp"
#include "raylib.h"

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
	SetTargetFPS(60);
	SetExitKey(0);		// Esc

	using namespace meteor;
	network::startup netboot;

	ip_address local_adress(192, 168, 0, 113);
	ip_endpoint local_endpoint(local_adress, 54321);
	udp_socket socket;
	if (!socket.open_and_bind(local_endpoint)) {
		print_error_code();
		return 0;
	}

	debug::info("local endpoint: %d.%d.%d.%d:%d",
		local_endpoint.address().a(),
		local_endpoint.address().b(),
		local_endpoint.address().c(),
		local_endpoint.address().d(),
		local_endpoint.port());

	const ip_address SERVER_IP = ip_address(10, 12, 163, 192);	
	ip_endpoint server_endpoint{ SERVER_IP , 54321};

	int my_x = 0, my_y = 0, server_x = 0, server_y = 0;

	int game_frame = 0;
	double prev_send_time = GetTime();
	const double TARGET_DELTA_MS = 0.5;

	bool running = true;
	while (running) {
		const float dt = GetFrameTime();
		running &= !WindowShouldClose();
		game_frame += 1;
		
		if (prev_send_time + TARGET_DELTA_MS < GetTime()) {
			prev_send_time = GetTime();

			mouse_position_message message((float)GetMouseX(), (float)GetMouseY());
			ping_message ping_message(GetTime());

			byte_stream stream_send;
			byte_stream_writer writer(stream_send);

			message.serialize<byte_stream_writer>(writer);
			ping_message.serialize<byte_stream_writer>(writer);

			const ip_endpoint TEACHER_ENDPOINT{ ip_address(192,168,0,101), 54321 };
			if (!socket.send_to(TEACHER_ENDPOINT, stream_send)) {
				print_error_code();
			}
		}
		

		// Recieve
		while (socket.has_data()){
			byte_stream stream_recieve;
			ip_endpoint sender_endpoint;
			if (socket.receive_from(sender_endpoint, stream_recieve)) {

				byte_stream_reader reader(stream_recieve);
				/*
				debug::info("%d - recieving from endpoint: %d.%d.%d.%d:%d, data size: %d",
					game_frame,
					sender_endpoint.address().a(),
					sender_endpoint.address().b(),
					sender_endpoint.address().c(),
					sender_endpoint.address().d(),
					sender_endpoint.port(),
					stream_recieve.size());
					*/

				uint8 type = reader.peek();

				switch(type){
				case((uint8)message_type::PING):
					ping_message message;
					if (!message.read(reader)) {
						print_error_code();
					}
					debug::info("latency: %f ", (GetTime() - message.m_time));
					break;
				}

					
			}
		}

		BeginDrawing();
		ClearBackground(SKYBLUE);

		DrawRectangle(my_x, my_y, 10, 10, GREEN);
		DrawRectangle(server_x, server_y, 10, 10, YELLOW);

		DrawFPS(2, 2);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
