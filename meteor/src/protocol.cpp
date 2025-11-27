// protocol.cpp

#include "protocol.hpp"

namespace meteor
{
	connect_packet::connect_packet()
		: m_type((uint8)protocol_packet_type::CONNECT)
		, m_magic(PROTOCOL_MAGIC)
		, m_version(PROTOCOL_VERSION)
		, m_player_id(0)
	{
	}

	connect_packet::connect_packet(uint8 id)
		: m_type((uint8)protocol_packet_type::CONNECT)
		, m_magic(PROTOCOL_MAGIC)
		, m_version(PROTOCOL_VERSION)
		, m_player_id(id)
	{
	}

	template <typename T>
	bool serialize(connect_packet& message, T& stream)
	{
		bool success = true;
		success &= stream.serialize(message.m_type);
		success &= stream.serialize(message.m_magic);
		success &= stream.serialize(message.m_version);
		success &= stream.serialize(message.m_player_id);
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

	discovery_packet::discovery_packet()
		: m_type((uint8)protocol_packet_type::DISCOVERY)
		, m_version(PROTOCOL_VERSION)
		, m_player_count(0)
		, m_is_response(false)
	{
	}

	discovery_packet::discovery_packet(uint8 player_count, bool is_response)
		: m_type((uint8)protocol_packet_type::DISCOVERY)
		, m_version(PROTOCOL_VERSION)
		, m_player_count(player_count)
		, m_is_response(is_response)
	{
	}

	template <typename T>
	bool serialize(discovery_packet& message, T& stream)
	{
		bool success = true;
		success &= stream.serialize(message.m_type);
		success &= stream.serialize(message.m_version);
		success &= stream.serialize(message.m_player_count);
		success &= stream.serialize(message.m_is_response);
		return success;
	}

	bool discovery_packet::write(byte_stream_writer& writer)
	{
		return serialize(*this, writer);
	}

	bool discovery_packet::read(byte_stream_reader& reader)
	{
		return serialize(*this, reader);
	}


	disconnect_packet::disconnect_packet(uint8 reason)//char message[256]
		: m_type((uint8)protocol_packet_type::DISCONNECT)
	{
	}

	template <typename T>
	bool serialize(disconnect_packet& message, T& stream)
	{
		bool success = true;
		success &= stream.serialize(message.m_type);
		success &= stream.serialize(message.m_reason);
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

	payload_packet::payload_packet(uint32 sequence, uint32 acknowledge)
		: m_type((uint8)protocol_packet_type::PAYLOAD)
		, m_sequence(sequence)
		, m_acknowledge(acknowledge)
	{
	}

	template <typename T>
	bool serialize(payload_packet& message, T& stream)
	{
		bool success = true;
		success &= stream.serialize(message.m_type);
		success &= stream.serialize(message.m_sequence);
		success &= stream.serialize(message.m_acknowledge);
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
