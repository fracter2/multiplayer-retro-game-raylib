// protocol.cpp

#include "protocol.hpp"

namespace meteor
{
	connect_packet::connect_packet()
		: m_type((uint8)protocol_packet_type::CONNECT)
		, m_magic(PROTOCOL_MAGIC)
		, m_version(PROTOCOL_VERSION)
	{
	}

	template <typename T>
	bool serialize(connect_packet& message, T& stream)
	{
		bool success = true;
		success &= stream.serialize(message.m_type);
		success &= stream.serialize(message.m_magic);
		success &= stream.serialize(message.m_version);
		return success;
	}

	bool connect_packet::write(byte_stream_writer& writer)
	{
		return serialize(*this, writer);
	}

	bool connect_packet::read(byte_stream_reader& reader)
	{
		return serialize(*this, reader);
	}

	disconnect_packet::disconnect_packet(uint8 reason, char message[256])
		: m_type((uint8)protocol_packet_type::DISCONNECT)
		, m_reason(reason)
	{
		int i = 0;
		for (char& m : m_message) {
			m = message[i];
		}
	}

	template <typename T>
	bool serialize(disconnect_packet& message, T& stream)
	{
		bool success = true;
		success &= stream.serialize(message.m_type);
		success &= stream.serialize(message.m_reason);
		success &= stream.serialize(message.m_message);
		return success;
	}

	bool disconnect_packet::write(byte_stream_writer& writer)
	{
		return serialize(*this, writer);
	}

	bool disconnect_packet::read(byte_stream_reader& reader)
	{
		return serialize(*this, reader);
	}

	payload_packet::payload_packet(uint32 sequence)
		: m_type((uint8)protocol_packet_type::PAYLOAD)
		, m_sequence(sequence)
	{
	}

	template <typename T>
	bool serialize(payload_packet& message, T& stream)
	{
		bool success = true;
		success &= stream.serialize(message.m_type);
		success &= stream.serialize(message.m_sequence);
		return success;
	}

	bool payload_packet::write(byte_stream_writer& writer)
	{
		return serialize(*this, writer);
	}

	bool payload_packet::read(byte_stream_reader& reader)
	{
		return serialize(*this, reader);
	}
} // !meteor
