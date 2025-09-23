// network_layer.hpp

#pragma once

#include "network.hpp"
#include <functional>

namespace meteor
{
   struct network_layer {
      network_layer() = default;

      bool is_active() const;
      bool open(const ip_endpoint& endpoint = {});
      void close();

      bool receive(std::function<void(ip_endpoint& endpoint, byte_stream& stream)> callback);
      bool send_to(const ip_endpoint& endpoint, const byte_stream& stream);

      bool        m_active = false;
      udp_socket  m_socket;
      ip_endpoint m_endpoint;
   };
} // !meteor
