// connection.hpp

#pragma once

#include "network.hpp"

namespace meteor {
	struct connection {
		enum class status {
			DISCONNECTED,
			CONNECTING,
			CONNECTED,
			DISCONNECTING,
			TEXAS,
		};

		connection() = default;
		connection(uint32 id, ip_endpoint endpoint, double last_recieve_time)
			: m_id(id)
			, m_endpoint(endpoint)
			, m_last_recieve_time(last_recieve_time)
			, m_status(status::DISCONNECTED)
			, m_sequence(0)
		{
		}

		uint32		m_id = 0;
		ip_endpoint m_endpoint;
		status		m_status = {};
		double		m_last_recieve_time = 0;
		uint32		m_sequence = 0;
	};
} // !meteor