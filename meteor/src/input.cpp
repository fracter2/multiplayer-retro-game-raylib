// input.cpp

#pragma once

#include "input.hpp"
#include "raylib.h"

namespace meteor::input {

	input_state::input_state(bool up, bool down, bool left, bool right, bool place_bomb, bool lmb, bool esc)
		: m_up(up)
		, m_down(down)
		, m_left(left)
		, m_right(right)
		, m_place_bomb(place_bomb)
		, m_lmb(lmb)
		, m_lmb_just_pressed(lmb)
		, m_esc(esc)
		, m_esc_just_pressed(esc)
	{
	}

	void update(input_state& state) {
		//input_state state;
		state.m_up = IsKeyDown(KEY_W) or IsKeyDown(KEY_UP);
		state.m_down = IsKeyDown(KEY_S) or IsKeyDown(KEY_DOWN);
		state.m_left = IsKeyDown(KEY_A) or IsKeyDown(KEY_LEFT);
		state.m_right = IsKeyDown(KEY_D) or IsKeyDown(KEY_RIGHT);
		state.m_place_bomb = IsKeyDown(KEY_SPACE);
		state.m_lmb_just_pressed = (!state.m_lmb) and IsMouseButtonDown(MOUSE_BUTTON_LEFT);
		state.m_lmb = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
		state.m_esc_just_pressed = (!state.m_esc) and IsKeyDown(KEY_ESCAPE);
		state.m_esc = IsKeyDown(KEY_ESCAPE);

		state.m_1_just_pressed = !(state.m_1) and IsKeyDown(KEY_ONE);			// Toggle server online / offline. Client quit-game
		state.m_1 = IsKeyDown(KEY_ONE);
		state.m_2_just_pressed = !(state.m_2) and IsKeyDown(KEY_TWO);			// Toggle server send broadcast / client join broadcast
		state.m_2 = IsKeyDown(KEY_TWO);
		state.m_3_just_pressed = !(state.m_3) and IsKeyDown(KEY_THREE);			// Server start game
		state.m_3 = IsKeyDown(KEY_THREE);
	}
}