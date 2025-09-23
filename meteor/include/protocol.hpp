// protocol.hpp

#pragma once

#include "network.hpp"

namespace meteor
{
	constexpr uint32 PROTOCOL_MAGIC     = 0xbaadf00d;
	constexpr uint32 PROTOCOL_VERSION   = 0x00010000;
	constexpr double TIMEOUT            = 1.0;

   enum class protocol_packet_type : uint8 {
	  CONNECT,
	  DISCONNECT,
	  PAYLOAD,
   };

   struct connect_packet {
	   connect_packet();

	   bool write(byte_stream_writer& writer);
	   bool read(byte_stream_reader& reader);

	   uint8 m_type = 0;
	   uint32 m_magic = 0;
	   uint32 m_version = 0; 
   };

   struct disconnect_packet {
	   disconnect_packet() = default;
	   disconnect_packet(uint8 reason); //, char message[256]

	   bool write(byte_stream_writer& writer);
	   bool read(byte_stream_reader& reader);

	   uint8 m_type = 0;
	   uint8 m_reason = 0;
	   //char m_message[256] = {};
   };

   struct payload_packet {
	   payload_packet() = default;
	   payload_packet(uint32 sequence);

	   bool write(byte_stream_writer& writer);
	   bool read(byte_stream_reader& reader);

	   uint8 m_type = 0;
	   uint32 m_sequence = 0;
   };
} // !meteor
