
// messages.hpp

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

		bool write(byte_stream_writer& writer) {
			bool success = true;
			success &= writer.serialize(m_x);
			success &= writer.serialize(m_y);
			return success;
		}

		float m_x = 0;
		float m_y = 0;
	};
} // !meteor