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
	const int window_width = 420, window_height = 360;
	const std::string_view window_title = "Drawing over IP client";
	InitWindow(window_width, window_height, window_title.data());
	SetTargetFPS(45);
	SetExitKey(0);		// Esc

	using namespace meteor;
	network::startup netboot;

	ip_address local_adress(10, 12, 210, 137);
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

	// my IP at-time-of-writing: 10.12.210.137
	// Sebbe IP at-time-of-writing: 10.12.163.192
	// designated port: 54321
	const ip_address SERVER_IP = ip_address(10, 12, 163, 192);	
	
	ip_endpoint server_endpoint{ SERVER_IP , 54321};

	std::string_view message = "Hello!";

	int my_x = 0, my_y = 0, server_x = 0, server_y = 0;

	int game_frame = 0;
	bool running = true;
	while (running) {
		const float dt = GetFrameTime();
		running &= !WindowShouldClose();
		
		game_frame += 1;

		// Send
		byte_stream stream_send;
		byte_stream_writer writer(stream_send);

		writer.serialize(GetMouseX());
		writer.serialize(GetMouseY());

		if (!socket.send_to(server_endpoint, stream_send)) {
			print_error_code();
		}

		// Recieve
		while (socket.has_data()){
			byte_stream stream_recieve;
			ip_endpoint sender_endpoint;
			if (socket.receive_from(sender_endpoint, stream_recieve)) {

				byte_stream_reader reader(stream_recieve);
				debug::info("%d - recieving from endpoint: %d.%d.%d.%d:%d, data size: %d",
					game_frame,
					sender_endpoint.address().a(),
					sender_endpoint.address().b(),
					sender_endpoint.address().c(),
					sender_endpoint.address().d(),
					sender_endpoint.port(),
					stream_recieve.size());

				// Check size of package. Server constantly sends own pos (8 bytes), but sends both their/mine less frequently.
				if (stream_recieve.size() > 8) {
					reader.serialize<int>(my_x);
					reader.serialize<int>(my_y);
				}
				reader.serialize<int>(server_x);
				reader.serialize<int>(server_y);	
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
