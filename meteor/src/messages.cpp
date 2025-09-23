// messages.cpp

#include "messages.hpp"

namespace meteor
{
   entity_state_message::entity_state_message(int32 id, Vector2 position, Color color)
      : m_type((uint8)message_type::ENTITY_STATE)
      , m_id(id)
      , m_position(position)
      , m_color(color)
   {
   }

   template <typename T>
   bool serialize(entity_state_message &message, T& stream)
   {
      bool success = true;
      success &= stream.serialize(message.m_type);
      success &= stream.serialize(message.m_id);
      success &= stream.serialize(message.m_position.x);
      success &= stream.serialize(message.m_position.y);
      success &= stream.serialize(message.m_color.r);
      success &= stream.serialize(message.m_color.g);
      success &= stream.serialize(message.m_color.b);
      success &= stream.serialize(message.m_color.a);
      return success;
   }

   bool entity_state_message::write(byte_stream_writer& writer)
   {
      return serialize(*this, writer);
   }

   bool entity_state_message::read(byte_stream_reader& reader)
   {
      return serialize(*this, reader);
   }

   latency_message::latency_message(double time)
      : m_type((uint8)message_type::LATENCY)
      , m_time(time)
   {
   }

   template <typename T>
   bool serialize(latency_message &message, T& stream)
   {
      bool success = true;
      success &= stream.serialize(message.m_type);
      success &= stream.serialize(message.m_time);
      return success;
   }

   bool latency_message::write(byte_stream_writer& writer)
   {
      return serialize(*this, writer);
   }

   bool latency_message::read(byte_stream_reader& reader)
   {
      return serialize(*this, reader);
   }

   mouse_position_message::mouse_position_message(float x, float y)
      : m_type((uint8)message_type::POSITION)
      , m_x(x)
      , m_y(y)
   {
   }

   template <typename T>
   bool serialize(mouse_position_message &message, T& stream)
   {
      bool success = true;
      success &= stream.serialize(message.m_type);
      success &= stream.serialize(message.m_x);
      success &= stream.serialize(message.m_y);
      return success;
   }

   bool mouse_position_message::write(byte_stream_writer& writer)
   {
      return serialize(*this, writer);
   }

   bool mouse_position_message::read(byte_stream_reader& reader)
   {
      return serialize(*this, reader);
   }

} // !meteor
