
// messages.hpp

#pragma once
#include "network.hpp"

namespace meteor 
{
	enum class message_type : uint8 {
		PING,
		POSITION
	};

	struct mouse_position_message {
		mouse_position_message() = default;
		mouse_position_message(float x, float y) 
			: m_type((uint8)message_type::POSITION)
			, m_x(x)
			, m_y(y)
		{
		}

		template<typename T>
		bool serialize(T& stream) {
			bool success = true;
			success &= stream.serialize(m_type);
			success &= stream.serialize(m_x);
			success &= stream.serialize(m_y);
			return success;
		}

		bool write(byte_stream_writer& writer) {
			return serialize(writer);
		}

		bool read(byte_stream_reader& reader) {
			return serialize(reader);
		}

		uint8 m_type = 0;
		float m_x = 0;
		float m_y = 0;
	};

	struct ping_message {
		ping_message() = default;
		ping_message(double time)
			: m_type((uint8)message_type::PING)
			, m_time(time) 
		{
		}

		template<typename T>
		bool serialize(T& stream) {
			bool success = true;
			success &= stream.serialize(m_type);
			success &= stream.serialize(m_x);
			return success;
		}

		bool write(byte_stream_writer& writer) {
			return serialize(writer);
		}

		bool read(byte_stream_reader& reader) {
			return serialize(reader);
		}

		uint8 m_type = 0;
		double m_time = 0;
	};

} // !meteor