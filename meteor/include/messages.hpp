
// messages.hpp

#pragma once
#include "network.hpp"

namespace meteor 
{
	struct mouse_position_message {
		mouse_position_message() = default;
		mouse_position_message(float x, float y) :
			m_x(x),
			m_y(y)
		{
		}

		template<typename T>
		bool serialize(T& stream) {
			bool success = true;
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

		float m_x = 0;
		float m_y = 0;
	};
} // !meteor