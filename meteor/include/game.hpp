// game.hpp

#pragma once

#include "raylib.h"
#include "network.hpp"
#include "messages.hpp"

namespace meteor {
	struct entity {
		entity() = default;
		entity(int32 id, Vector2 position, Color color)
			: m_id(id)
			, m_position(position)
			, m_color(color)
		{
		}
		int32 m_id = 0;
		Vector2 m_position = {};
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

		std::vector<entity> m_entities = {};

	};
}
