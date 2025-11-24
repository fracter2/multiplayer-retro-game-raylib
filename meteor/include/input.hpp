// input.hpp

#pragma once

#include "common.hpp"


namespace meteor::input {

	/*
	enum class input_type : uint8 {
		LATENCY,
		POSITION,
		ENTITY_STATE
	};
	*/

	struct input_state {
		input_state() = default;

		input_state(bool up, bool down, bool left, bool right, bool place_bomb, bool lmb, bool esc);

		/* TODO make it use induvidual bits like below, to save space. low prio as it's shouldn't be sent over network anyway.
		uint8 m_bits = 0;

		void set_true(input_type input) {
			uint8 bitmask = 1 << (uint8)input;			// x << y is bit shifting x left by y bits
			m_bits | bitmask;							// | is bitwise OR. To set bitmasked bits to 1
		}

		void set_false(input_type input) {
			uint8 bitmask = ~(1 << (uint8)input);		// ~ is bitwise "complement" operator, flips all 1 and 0
			m_bits & bitmask;							// & is bitwise AND. To flip (un-)bitmasked value to 0
		}
		*/

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

		bool m_2 = false;						// Client print debug info mid-game
		bool m_2_just_pressed = false;			// Server toggle broadcasts

		bool m_3 = false;						// Server start game.
		bool m_3_just_pressed = false;			// 

		bool m_4 = false;						// Server fill lobby with bots
		bool m_4_just_pressed = false;			// 
	};


	void update(input_state& state);
}