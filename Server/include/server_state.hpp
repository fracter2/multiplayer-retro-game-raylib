// server_State.hpp

#pragma once

#include "common.hpp"
#include "network.hpp"
#include "protocol.hpp"
#include "messages.hpp"
#include "protocol.hpp"
#include "connection.hpp"
#include "game.hpp"

namespace meteor {

	struct server_state {
		enum class status : uint8 {
			OFFLINE,
			ONLINE_JOINABLE,
			ONLINE
		};

		server_state() = default;

		bool has_client(const ip_endpoint& endpoint, uint8& r_index) {
			uint8 i = 0;
			for (connection& conn : m_clients) {
				if (conn.m_endpoint == endpoint
				&& conn.m_status != connection::status::DISCONNECTED) { 
					r_index = i;
					return true; 
				}
				i++;
			}
			return false;
		}

		/*
		uint8 get_client_index(const ip_endpoint& endpoint) const {
			uint8 i = 0;
			for (const connection& conn : m_clients) {
				if (conn.m_endpoint == endpoint
					&& conn.m_status != connection::status::DISCONNECTED) {
					return i;
				}
				i++;
			}

			assert(false);	// To have type safety, we expect "has_client()" to check before this method
			return -1;
		}
		*/

		// Used to know if the sender of a pack was a resently-left member. To be *extra* graceful, in case of packet loss.
		bool has_client_recently_left(const ip_endpoint& endpoint) {
			for (connection& conn : m_clients) {
				if (conn.m_endpoint == endpoint
					&& conn.m_status == connection::status::DISCONNECTED) {
					return true;
				}
			}
			return false;
		}

		bool m_broadcast = true;
		status m_status = status::OFFLINE;
		connection m_clients[game::MAX_PLAYERS] = {};
	};
}