// protocol.hpp

#pragma once

#include "network.hpp"


// TODO MOVE TO "meteor::network" or similar
namespace meteor
{
	constexpr uint32 PROTOCOL_MAGIC     = 0xbaadf00d;
	constexpr uint32 PROTOCOL_VERSION   = 0x00010000;
	constexpr double TIMEOUT            = 10.0;

   enum class protocol_packet_type : uint8 {
	  CONNECT,
	  DISCONNECT,
	  PAYLOAD,
	  MAX		// Used to check if its out-of-range. Keep as highest value
   };

   enum class disconnect_reason : uint8 {
	   SOMETHING,
	   TIMEOUT
   };

   struct connect_packet {
	   connect_packet();
	   connect_packet(uint8 id);

	   bool write(byte_stream_writer& writer);
	   bool read(byte_stream_reader& reader);

	   uint8 m_type = (uint8)protocol_packet_type::CONNECT;
	   uint32 m_magic = 0;
	   uint32 m_version = 0; 
	   uint8 m_player_id = 0;			// client player index, or when sent from server as broadcast: players already in lobby
   };

   struct disconnect_packet {
	   disconnect_packet() = default;
	   disconnect_packet(uint8 reason); //, char message[256]

	   bool write(byte_stream_writer& writer);
	   bool read(byte_stream_reader& reader);

	   uint8 m_type = (uint8)protocol_packet_type::DISCONNECT;
	   uint8 m_reason = 0;
	   //char m_message[256] = {};
   };

   struct payload_packet {
	   payload_packet() = default;
	   payload_packet(uint32 sequence, uint32 acknowledge);

	   bool write(byte_stream_writer& writer);
	   bool read(byte_stream_reader& reader);

	   uint8 m_type = (uint8)protocol_packet_type::PAYLOAD;
	   uint32 m_sequence = 0;
	   uint32 m_acknowledge = 0;
   };
} // !meteor
