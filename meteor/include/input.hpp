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

		bool m_up;
		bool m_down;
		bool m_left;
		bool m_right;
		bool m_place_bomb;
		bool m_lmb;
		bool m_lmb_just_pressed;
		bool m_esc;
		bool m_esc_just_pressed;
	};


	void update(input_state& state);
}