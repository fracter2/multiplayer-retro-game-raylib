// main.cpp

#include <thread>
#include <chrono>
#include <cstdio>

#include <stdio.h>
#include "raylib.h"
#include "timer.hpp"
#include "network.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "connection.hpp"

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
	//SetTargetFPS(60);
	SetExitKey(0);		// Esc

    using namespace meteor;
    network::startup boot;

    const ip_endpoint LOCAL_ENDPOINT    (ip_address(192, 168, 1, 255),  54321);
    const ip_endpoint SERVER_ENDPOINT   (ip_address(192, 168, 1, 72),   54321);

    connection server_connection;
    server_connection.m_status = connection::status::CONNECTING;

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

    //int game_frame = 0;
    double prev_send_time = GetTime();
    const double TARGET_DELTA_MS = 0.5;

    bool running = true;
    while (running) {
        const float dt = GetFrameTime();
        running &= !WindowShouldClose();
        

        // note: network update
        if (prev_send_time + TARGET_DELTA_MS < GetTime()) {
            prev_send_time = GetTime();

            byte_stream stream_send;
            byte_stream_writer writer(stream_send);

            switch (server_connection.m_status) {
            case(connection::status::CONNECTING):
                connect_packet message;
                message.write(writer);
                if (!socket.send_to(SERVER_ENDPOINT, stream_send)) { print_error_code(); }
                break;

            case (connection::status::CONNECTED):
                break;
            }
            
            mouse_position_message message((float)GetMouseX(), (float)GetMouseY());
            latency_message ping_message(GetTime());

            message.write(writer);
            ping_message.write(writer);
        } // !network update

        // note: recieve data
        while (socket.has_data()) {
            byte_stream stream_recieve;
            ip_endpoint sender_endpoint;
            if (!socket.receive_from(sender_endpoint, stream_recieve)) { break; }
            if (sender_endpoint != SERVER_ENDPOINT) {
                debug::info("%i - !! recieving from NON-SERVER endpoint: %d.%d.%d.%d:%d, data size: %d",
                    GetTime(),
                    sender_endpoint.address().a(),
                    sender_endpoint.address().b(),
                    sender_endpoint.address().c(),
                    sender_endpoint.address().d(),
                    sender_endpoint.port(),
                    stream_recieve.size());
                break;
            }

            byte_stream_reader reader(stream_recieve);
            debug::info("%i - recieving from server, data size: %d",
                GetTime(),
                stream_recieve.size());
            

            uint8 protocol = reader.peek();

            switch (protocol) {
            case((uint8)protocol_packet_type::CONNECT):
            {
                connect_packet packet;
                if (!packet.read(reader)) { print_error_code(); break; }
                if (server_connection.m_status == connection::status::CONNECTING) {
                    server_connection.m_status = connection::status::CONNECTED;
                    server_connection.m_last_recieve_time = GetTime();
                    debug::info("%i - now connected to server", GetTime());
                }
                else {
                    debug::info("%i - recieved connect package when irrellevant", GetTime());
                }
                break;
            }

            case((uint8)protocol_packet_type::DISCONNECT):
            {
                disconnect_packet packet;
                if (!packet.read(reader)) { print_error_code(); break; }
                server_connection.m_last_recieve_time = GetTime();

                if (server_connection.m_status == connection::status::DISCONNECTING)
                    debug::info("%i - Gracefully disconnected", GetTime());
                else if (server_connection.m_status == connection::status::DISCONNECTED)
                    debug::info("%i - recived disconnect package when already disconneced", GetTime());
                else
                    debug::info("%i - Disgracefull disconnect", GetTime());

                server_connection.m_status = connection::status::DISCONNECTED;
                break;
            }

            case ((uint8)protocol_packet_type::PAYLOAD):
            {
                payload_packet packet;
                if (!packet.read(reader)) { print_error_code(); break; }
                server_connection.m_last_recieve_time = GetTime();

                uint8 type = reader.peek();

                switch (type) {
                case ((uint8)message_type::LATENCY): {
                    latency_message message;
                    if (!message.read(reader)) { print_error_code(); break; }
                    debug::info("latency: %f ", (GetTime() - message.m_time));
                    break;
                }

                case ((uint8)message_type::ENTITY_STATE): {
                    entity_state_message message;
                    if (!message.read(reader)) { print_error_code(); break; }

                    break;
                } 
                } // !payload switch
            }
            } // !protocol switch
                
            break;
                
            
        } // !while socket.has_data()

        // DRAWING
        BeginDrawing();
        ClearBackground(SKYBLUE);

        // TODO DRAW ALL ENTITIES W COLOR
        //DrawRectangle(my_x, my_y, 10, 10, GREEN);
        //DrawRectangle(server_x, server_y, 10, 10, YELLOW);

        DrawFPS(2, 2);
        EndDrawing();

        // note: save the forest!
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    CloseWindow();

    return 0;
}
