// messages.hpp

#pragma once

#include "raylib.h"
#include "network.hpp"

namespace meteor
{
   enum class message_type : uint8 {
      LATENCY,
      POSITION,
      ENTITY_STATE
   };

   struct entity_state_message {
      entity_state_message() = default;
      entity_state_message(int32 id, Vector2 position, Color color);

      bool write(byte_stream_writer& writer);
      bool read(byte_stream_reader& reader);
      
      uint8   m_type = 0;
      int32   m_id = 0;
      Vector2 m_position = {};
      Color   m_color = {};
   };

   struct latency_message {
      latency_message() = default;
      latency_message(double time);

      bool write(byte_stream_writer& writer);
      bool read(byte_stream_reader& reader);

      uint8  m_type = 0;
      double m_time = 0;
   };

   struct mouse_position_message {
      mouse_position_message() = default;
      mouse_position_message(float x, float y);

      bool write(byte_stream_writer& writer);
      bool read(byte_stream_reader& reader);

      uint8 m_type = 0;
      float m_x = 0;
      float m_y = 0;
   };
} // !meteor
