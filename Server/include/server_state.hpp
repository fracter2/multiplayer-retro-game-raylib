// server_State.hpp

#pragma once

#include "common.hpp"
#include "network.hpp"
#include "protocol.hpp"
#include "messages.hpp"
#include "connection.hpp"
#include "game.hpp"

namespace meteor {

	struct server_state {
		static constexpr uint32 BROADCAST_IDLE_TICKS = 120;

		enum class status : uint8 {
			OFFLINE,
			ONLINE_JOINABLE,
			ONLINE
		};

		server_state() = default;

		bool has_client(const ip_endpoint& endpoint, uint32& r_index) {
			uint32 i = 0;
			for (connection& conn : m_clients) {
				if (conn.m_endpoint == endpoint
				&& conn.get_status() != connection::status::DISCONNECTED) { 
					r_index = i;
					return true; 
				}
				i++;
			}
			return false;
		}

		bool has_client(const ip_endpoint& endpoint) {
			for (connection& conn : m_clients) {
				if (conn.m_endpoint == endpoint
					&& conn.get_status() != connection::status::DISCONNECTED) {
					return true;
				}
			}
			return false;
		}

		// Used to know if the sender of a pack was a resently-left member. To be *extra* graceful, in case of packet loss.
		bool has_client_recently_left(const ip_endpoint& endpoint) {
			for (connection& conn : m_clients) {
				if (conn.m_endpoint == endpoint
					&& conn.get_status() == connection::status::DISCONNECTED) {
					return true;
				}
			}
			return false;
		}

		uint32 get_client_count() const {
			uint32 i = 0;
			for (const connection& conn : m_clients) {
				if (conn.get_status() != connection::status::DISCONNECTED) { i++; }
			}
			return i;
		}

		bool m_debug_skip_recieve = false;
		bool m_broadcast = true;
		uint32 m_next_broadcast_tick = 0;
		status m_status = status::OFFLINE;
		connection m_clients[MAX_PLAYERS] = {};
	};
}