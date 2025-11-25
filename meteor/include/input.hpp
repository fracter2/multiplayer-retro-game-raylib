// input.hpp

#pragma once

#include "common.hpp"


namespace meteor {

	struct input_state {
		input_state() = default;

		void update();

		bool m_up = false;
		bool m_down = false;
		bool m_left = false;
		bool m_right = false;
		bool m_place_bomb = false;
		bool m_lmb = false;
		bool m_lmb_just_pressed = false;
		bool m_esc = false;
		bool m_esc_just_pressed = false;
												// What each key does for server & client, aligned with/without "just_pressed" in mind
												//
		bool m_1 = false;						// 
		bool m_1_just_pressed = false;			// Server toggle online / offline. Client quit-game

		bool m_2 = false;						// (press or hold) Client print debug info mid-game
		bool m_2_just_pressed = false;			// Server toggle broadcasts

		bool m_3 = false;						// 
		bool m_3_just_pressed = false;			// Server start game.

		bool m_4 = false;						// 
		bool m_4_just_pressed = false;			// Server fill lobby with bots

		bool m_5 = false;						// Server & Client (hold) Skip any recieve packet (simulate packet loss)
		bool m_5_just_pressed = false;			// 

		bool m_6 = false;						// 
		bool m_6_just_pressed = false;			// Server Toggle sending fewer game state updates (3 * 20/s default -> 1 * 20/s)

		bool m_7 = false;						// 
		bool m_7_just_pressed = false;			// Server & client lessen time to next tick // TODO

		bool m_8 = false;						// 
		bool m_8_just_pressed = false;			// Server & client increase time to next tick // TODO

		bool m_9 = false;						// 
		bool m_9_just_pressed = false;			// Server & client Allow instant ACK replies // TODO
	};


	
}