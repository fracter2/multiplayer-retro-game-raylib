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

	struct game {
		game() = default;

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

			double lerp_fraction = (GetTime() - m_prev_time_sec) / NETWORK_TICK_SECONDS;
			lerp_fraction = (double)Clamp((float)lerp_fraction, 0.0f, 1.0f);

			//std::vector<Vector2> lerped_positions = {};


			for (int i = 0; i < m_entities.size(); i++) {
				Vector2 pos = Vector2Lerp(
					m_entities[i].m_position_prev, 
					m_entities[i].m_position, 
					(float)lerp_fraction);

				DrawRectangle((int)pos.x, (int)pos.y, 10, 10, m_entities[i].m_color);
			}

			DrawFPS(2, 2);
		}

		std::vector<entity> m_entities = {};
		double m_prev_time_sec = 0;


	};
}
