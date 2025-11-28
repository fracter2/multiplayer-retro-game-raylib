// input.cpp

#pragma once

#include "input.hpp"
#include "raylib.h"

namespace meteor {


	void input_state::update() {
		m_up_just_pressed = !(m_up) and (IsKeyDown(KEY_W) or IsKeyDown(KEY_UP));
		m_up = IsKeyDown(KEY_W) or IsKeyDown(KEY_UP);

		m_down_just_pressed = !(m_down) and (IsKeyDown(KEY_S) or IsKeyDown(KEY_DOWN));
		m_down = IsKeyDown(KEY_S) or IsKeyDown(KEY_DOWN);

		m_left = IsKeyDown(KEY_A) or IsKeyDown(KEY_LEFT);
		m_right = IsKeyDown(KEY_D) or IsKeyDown(KEY_RIGHT);

		m_place_bomb_just_pressed = !(m_place_bomb) and (IsKeyDown(KEY_SPACE));
		m_place_bomb = IsKeyDown(KEY_SPACE);

		m_lmb_just_pressed = (!m_lmb) and IsMouseButtonDown(MOUSE_BUTTON_LEFT);
		m_lmb = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

		m_esc_just_pressed = (!m_esc) and IsKeyDown(KEY_ESCAPE);
		m_esc = IsKeyDown(KEY_ESCAPE);

		m_1_just_pressed = !(m_1) and IsKeyDown(KEY_ONE);
		m_1 = IsKeyDown(KEY_ONE);
		m_2_just_pressed = !(m_2) and IsKeyDown(KEY_TWO);
		m_2 = IsKeyDown(KEY_TWO);
		m_3_just_pressed = !(m_3) and IsKeyDown(KEY_THREE);	
		m_3 = IsKeyDown(KEY_THREE);
		m_4_just_pressed = !(m_4) and IsKeyDown(KEY_FOUR);
		m_4 = IsKeyDown(KEY_FOUR);
		m_5_just_pressed = !(m_5) and IsKeyDown(KEY_FIVE);
		m_5 = IsKeyDown(KEY_FIVE);
		m_6_just_pressed = !(m_6) and IsKeyDown(KEY_SIX);
		m_6 = IsKeyDown(KEY_SIX);
		m_7_just_pressed = !(m_7) and IsKeyDown(KEY_SEVEN);
		m_7 = IsKeyDown(KEY_SEVEN);
		m_8_just_pressed = !(m_8) and IsKeyDown(KEY_EIGHT);
		m_8 = IsKeyDown(KEY_EIGHT);
		m_9_just_pressed = !(m_9) and IsKeyDown(KEY_NINE);
		m_9 = IsKeyDown(KEY_NINE);
	}
}