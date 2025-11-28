// server_browser.hpp

#pragma once

#include "common.hpp"
#include "network.hpp"

namespace meteor {

	

	struct server_browser {

		struct entry {
			entry(ip_endpoint endpoint, size_t player_count) noexcept 
				: m_endpoint(endpoint)
				, m_players(player_count)
			{
			};

			ip_endpoint m_endpoint = {};
			size_t m_players = 0;
		};


		server_browser() = default;

		void reset() {
			m_entries.clear();
			m_select_index = 0;
		}

		std::vector<entry> m_entries = {};
		bool m_queue_discovery_send = false;
		int m_select_index = 0;

	};
}