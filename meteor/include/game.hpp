// game.hpp

#pragma once

#include "raylib.h"
#include "raymath.h"
#include "network.hpp"
#include "messages.hpp"

namespace meteor {
	constexpr double NETWORK_TICK_SECONDS = 2.0;

	struct entity {
		entity() = default;
		entity(int32 id, Vector2 position, Color color)
			: m_id(id)
			, m_position(position)
			, m_position_prev(position)
			, m_color(color)
		{
		}
		int32 m_id = 0;
		Vector2 m_position = {};
		Vector2 m_position_prev = {};
		Color m_color = {};
	};

	// TODO SEPPARATE GAME_STATES INTO GAME_STATE AND LATENCY_STATES (with input history)
	// TODO SEPARATE UPDATE LOGIC FROM CONTAINER
	struct game {
		game() = default;

		// TODO Should be FIXED TIMESTEP delta at 60 or 64 ticks a second
		void update_process() {
			m_tick += 1;

			// TODO Update entities. DOES INPUT NEED TO BE HERE? OR CAN IT BE SAMLED WITH NETWORKING?
		}

		void update_entity(entity_state_message message) {

			int index = get_entity_index(message.m_id);
			if (index == -1) {
				add_entity(entity(message.m_id, message.m_position, message.m_color));
			}
			else {
				m_entities[index].m_position_prev = m_entities[index].m_position;
				m_entities[index].m_position = message.m_position;
				m_entities[index].m_color = message.m_color;
			}
		}

		// returns -1 on not found
		int get_entity_index(int32 id) const {
			for (int i = 0; i < m_entities.size(); i++) {
				if (m_entities[i].m_id == id) return i;
			}

			return -1;
		}

		void add_entity(entity entity) {

			m_entities.push_back(entity);
		}

		void render_frame() {
			ClearBackground(SKYBLUE);

			float lerp_fraction = (float)((GetTime() - m_time_sec) / NETWORK_TICK_SECONDS);
			lerp_fraction = Clamp(lerp_fraction, 0.0f, 1.0f);

			for (int i = 0; i < m_entities.size(); i++) {
				Vector2 pos = Vector2Lerp(
					m_entities[i].m_position_prev, 
					m_entities[i].m_position, 
					lerp_fraction);

				DrawRectangleV(pos, Vector2(10, 10), m_entities[i].m_color);
			}

			DrawFPS(2, 2);
		}

		std::vector<entity> m_entities = {};
		double m_time_sec = 0;
		int32 m_tick = 0;
	};
}
