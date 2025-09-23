// network_layer.cpp

#include "network_layer.hpp"

namespace meteor
{
   bool network_layer::is_active() const
   {
      return m_active;
   }

   bool network_layer::open(const ip_endpoint& endpoint)
   {
      m_active = m_socket.open_and_bind(endpoint);
      m_endpoint = endpoint;

      return m_active;
   }

   void network_layer::close()
   {
      m_socket.close();
   }

   bool network_layer::receive(std::function<void(ip_endpoint& endpoint, byte_stream& stream)> callback)
   {
      if (!m_socket.valid()) {
         return false;
      }

      ip_endpoint endpoint;
      byte_stream stream;
      if (m_socket.receive_from(endpoint, stream)) {
         callback(endpoint, stream);
         return true;
      }

      auto error = network::get_last_error();
      if (error.is_critical()) {
         assert(false);
      }

      return false;
   }

   bool network_layer::send_to(const ip_endpoint& endpoint, const byte_stream& stream)
   {
      if (!m_socket.valid()) {
         return false;
      }

      if (m_socket.send_to(endpoint, stream)) {
         return true;
      }
    
      auto error = network::get_last_error();
      if (error.is_critical()) {
         assert(false);
      }

      return false;
   }
} // !meteor
