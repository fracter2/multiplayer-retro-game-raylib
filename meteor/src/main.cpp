// main.cpp

#include <stdio.h>
#include <thread>
#include <chrono>
#include "network.hpp"
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
	const int window_width = 420, window_height = 360;
	const std::string_view window_title = "Drawing over IP client";
	InitWindow(window_width, window_height, window_title.data());
	SetTargetFPS(60);
	SetExitKey(0);		// Esc

	using namespace meteor;
	network::startup netboot;

	ip_endpoint local_endpoint;
	udp_socket socket;
	if (!socket.open_and_bind(local_endpoint)) {
		print_error_code();
		return 0;
	}

	debug::info("local endpoint: %d.%d.%d.%d:%d",
		local_endpoint.address().a(),
		local_endpoint.address().a(),
		local_endpoint.address().a(),
		local_endpoint.address().a(),
		local_endpoint.port());

	// my IP at-time-of-writing: 10.12.210.64
	// Sebbe IP at-time-of-writing: 10.12.210.67
	// designated port: 54321
	const ip_address SERVER_IP = ip_address(10, 12, 210, 67);	
	
	ip_endpoint server_endpoint{ SERVER_IP , 54321};

	std::string_view message = "Hello!";
	//int x = 101;
	//int y = 200;

	bool running = true;
	while (running) {
		const float dt = GetFrameTime();
		running &= !WindowShouldClose();
		
		byte_stream stream;
		byte_stream_writer writer(stream);

		

		writer.serialize(GetMouseX());
		writer.serialize(GetMouseY());
		for (auto ch : message) {
			writer.serialize(ch);
		}

		
		if (!socket.send_to(server_endpoint, stream)) {
			print_error_code();
		}

		BeginDrawing();
		ClearBackground(SKYBLUE);
		
		// draw a square on mouse pos

		DrawFPS(2, 2);
		EndDrawing();

		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	CloseWindow();

	return 0;
}
