// network.cpp

#include "network.hpp"
#include <chrono>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

namespace meteor
{
   namespace debug
   {
      void info(const char *format, ...)
      {
         char msg[1024] = {};
         va_list args;
         va_start(args, format);
         int len = vsprintf_s(msg, format, args);
         va_end(args);
         printf("NFO: %s\n", msg);
      }

      void warn(const char *format, ...)
      {
         char msg[1024] = {};
         va_list args;
         va_start(args, format);
         int len = vsprintf_s(msg, format, args);
         va_end(args);
         printf("WRN: %s\n", msg);
      }

      void error(const char *format, ...)
      {
         char msg[1024] = {};
         va_list args;
         va_start(args, format);
         int len = vsprintf_s(msg, format, args);
         va_end(args);
         printf("ERR! %s\n", msg);

         //assert(false);
      }
   } // !debug


   namespace time
   {
      int64 get_current_time_ms()
      {
         static int64 start = 0;
         if (start == 0) {
            auto ns = std::chrono::high_resolution_clock::now();
            auto us = std::chrono::time_point_cast<std::chrono::microseconds>(ns);
            start = us.time_since_epoch().count();
         }

         auto ns = std::chrono::high_resolution_clock::now();
         auto us = std::chrono::time_point_cast<std::chrono::microseconds>(ns);
         auto now = us.time_since_epoch().count();

         return (now - start);
      }
   } // !clock

   void byte_stream::reset()
   {
      m_size = 0;
   }

   bool byte_stream::has_data() const
   {
      return m_size > 0;
   }

   bool byte_stream::can_fit(int32 bytes) const
   {
      return (m_size + bytes) <= sizeof(m_data);
   }

   int32 byte_stream::size() const
   {
      return m_size;
   }

   uint8 *byte_stream::data()
   {
      return m_data;
   }

   const uint8 *byte_stream::data() const
   {
      return m_data;
   }

   byte_stream_writer::byte_stream_writer(byte_stream &stream)
      : m_stream(stream)
      , m_cursor(stream.m_data + stream.m_size)
      , m_end(stream.m_data + sizeof(stream.m_data))
   {
   }

   byte_stream_reader::byte_stream_reader(byte_stream &stream)
      : m_stream(stream)
      , m_cursor(stream.m_data)
      , m_end(stream.m_data + stream.m_size)
   {
   }

   bool byte_stream_reader::has_data() const
   {
      return m_cursor < m_end;
   }

   uint8 byte_stream_reader::peek() const
   {
      assert(has_data());
      return m_cursor[0];
   }

   struct subnet_mask {
      uint8 a = 0;
      uint8 b = 0;
      uint8 c = 0;
      uint8 d = 0;
   };

   error_code::error_code(int32 code)
      : m_code(code)
   {
   }

   int32 error_code::code() const
   {
      return m_code;
   }

   const char *error_code::c_str() const
   {
      return string().data();
   }

   std::string_view error_code::string() const
   {
      switch (m_code) {
         case WSAEADDRINUSE:
            return "WSAEADDRINUSE: Address already in use.";
         case WSAECONNRESET:
            return "WSAECONNRESET: Connection reset by peer.";
         case WSAEWOULDBLOCK:
            return "WSAEWOULDBLOCK: Resource temporarily unavailable.";
         case WSANOTINITIALISED:
            return "WSANOTINITIALISED: Successful WSAStartup not yet performed.";
      }

      return "Unknown socket error!";
   }

   bool error_code::is_critical() const
   {
      if (m_code == 0 ||
          m_code == WSAEWOULDBLOCK ||
          m_code == WSAECONNRESET)
      {
         return false;
      }

      return true;
   }

   ip_address::ip_address(uint32 host)
      : m_host(host)
   {
   }

   ip_address::ip_address(uint8 a, uint8 b, uint8 c, uint8 d)
      : m_host(0)
   {
      m_host |= uint32(a) << 24;
      m_host |= uint32(b) << 16;
      m_host |= uint32(c) << 8;
      m_host |= uint32(d) << 0;
   }

   bool ip_address::operator==(const ip_address &rhs) const
   {
      return m_host == rhs.m_host;
   }

   bool ip_address::operator!=(const ip_address &rhs) const
   {
      return m_host != rhs.m_host;
   }

   void ip_address::a(uint8 value)
   {
      m_host |= (m_host & 0x00ffffff);
      m_host |= uint32(value) << 24;
   }

   void ip_address::b(uint8 value)
   {
      m_host |= (m_host & 0xff00ffff);
      m_host |= uint32(value) << 16;
   }

   void ip_address::c(uint8 value)
   {
      m_host |= (m_host & 0xffff00ff);
      m_host |= uint32(value) << 8;
   }

   void ip_address::d(uint8 value)
   {
      m_host |= (m_host & 0xffffff00);
      m_host |= uint32(value);
   }

   uint8 ip_address::a() const
   {
      return (m_host >> 24) & 0xff;
   }

   uint8 ip_address::b() const
   {
      return (m_host >> 16) & 0xff;
   }

   uint8 ip_address::c() const
   {
      return (m_host >> 8) & 0xff;
   }

   uint8 ip_address::d() const
   {
      return (m_host >> 0) & 0xff;
   }

   ip_endpoint::ip_endpoint(const ip_address &address, uint16 port)
      : m_address(address)
      , m_port(port)
   {
   }

   bool ip_endpoint::operator==(const ip_endpoint &rhs) const
   {
      return m_address == rhs.m_address && m_port == rhs.m_port;
   }

   bool ip_endpoint::operator!=(const ip_endpoint &rhs) const
   {
      return m_address != rhs.m_address || m_port != rhs.m_port;
   }

   ip_address &ip_endpoint::address()
   {
      return m_address;
   }

   uint16 ip_endpoint::port() const
   {
      return m_port;
   }

   udp_socket::udp_socket()
      : m_handle(INVALID_SOCKET)
   {
   }

   bool udp_socket::valid() const
   {
      return m_handle != INVALID_SOCKET;
   }

   bool udp_socket::open()
   {
      return open_and_bind({});
   }

   bool udp_socket::open_and_bind(const ip_endpoint &endpoint)
   {
      SOCKET fd = ::socket(AF_INET, SOCK_DGRAM, 0);
      if (fd == INVALID_SOCKET) {
         return false;
      }

      sockaddr_in addr{};
      addr.sin_family = AF_INET;
      addr.sin_port = ::htons(endpoint.m_port);
      addr.sin_addr.s_addr = ::htonl(endpoint.m_address.m_host);
      if (::bind(fd, (const sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
         ::closesocket(fd);
         return false;
      }

      u_long non_blocking = 1;
      if (::ioctlsocket(fd, FIONBIO, &non_blocking) == SOCKET_ERROR) {
         ::closesocket(fd);
         return false;
      }

      m_handle = fd;

      return valid();
   }

   void udp_socket::close()
   {
      if (valid()) {
         ::closesocket(m_handle);
      }

      m_handle = INVALID_SOCKET;
   }

   bool udp_socket::has_data() const
   {
      if (!valid()) {
         return false;
      }

      u_long count = 0;
      if (::ioctlsocket(m_handle, FIONREAD, &count) == SOCKET_ERROR) {
         return false;
      }

      return count > 0;
   }

   bool udp_socket::send_to(const ip_endpoint &endpoint, const byte_stream &stream)
   {
      if (!valid()) {
         return false;
      }

      const int32 length = stream.m_size;
      const char *data = (const char *)stream.m_data;

      sockaddr_in addr{};
      addr.sin_family = AF_INET;
      addr.sin_port = ::htons(endpoint.m_port);
      addr.sin_addr.s_addr = ::htonl(endpoint.m_address.m_host);
      if (::sendto(m_handle, data, length, 0, (const sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
         return false;
      }

      return true;
   }

   bool udp_socket::receive_from(ip_endpoint &endpoint, byte_stream &stream)
   {
      if (!valid()) {
         return false;
      }

      sockaddr_in addr{};
      int32 addrlen = sizeof(addr);
      int32 result = ::recvfrom(m_handle, (char *)stream.m_data, sizeof(stream.m_data), 0, (sockaddr *)&addr, &addrlen);
      if (result == SOCKET_ERROR) {
         return false;
      }

      endpoint.m_address.m_host = ::ntohl(addr.sin_addr.s_addr);
      endpoint.m_port = ::ntohs(addr.sin_port);
      stream.m_size = result;

      return true;
   }

   namespace network
   {
      startup::startup()
      {
         WSADATA data = {};
         int ret = ::WSAStartup(MAKEWORD(2, 2), &data);
         if (ret == 0) {
            (void)ret;
         }
      }

      startup::~startup()
      {
         ::WSACleanup();
      }

      error_code get_last_error()
      {
         return error_code{ ::WSAGetLastError() };
      }

      ip_address resolve_address(std::string_view address)
      {
         addrinfo hints{};
         hints.ai_family = AF_UNSPEC;
         hints.ai_socktype = SOCK_STREAM;
         hints.ai_protocol = IPPROTO_TCP;

         addrinfo *result{ nullptr };
         int res = ::getaddrinfo(address.data(), nullptr, &hints, &result);
         if (res == WSAHOST_NOT_FOUND) {
            return ip_address{};
         }

         uint32 host = 0;
         for (addrinfo *it = result; it != nullptr; it = it->ai_next) {
            if (it->ai_family == AF_INET) {
               sockaddr_in &addr = *(sockaddr_in *)it->ai_addr;
               host = ::ntohl(addr.sin_addr.s_addr);
               break;
            }
         }

         ::freeaddrinfo(result);

         return ip_address{ host };
      }

      bool query_local_addresses(std::vector<ip_address> &addresses)
      {
         DWORD size = 0;
         ::GetAdaptersAddresses(AF_INET,
                                GAA_FLAG_INCLUDE_PREFIX,
                                NULL,
                                NULL,
                                &size);

         IP_ADAPTER_ADDRESSES *adapter_addresses = (IP_ADAPTER_ADDRESSES *)calloc(1, size);
         ::GetAdaptersAddresses(AF_INET,
                                GAA_FLAG_INCLUDE_PREFIX,
                                NULL,
                                adapter_addresses,
                                &size);

         for (IP_ADAPTER_ADDRESSES *iter = adapter_addresses; iter != NULL; iter = iter->Next) {
            if (iter->OperStatus == IfOperStatusUp && (iter->IfType == IF_TYPE_ETHERNET_CSMACD ||
                                                       iter->IfType == IF_TYPE_IEEE80211))
            {
               for (IP_ADAPTER_UNICAST_ADDRESS *ua = iter->FirstUnicastAddress; ua != NULL; ua = ua->Next) {
                  char addrstr[1024] = {};
                  ::getnameinfo(ua->Address.lpSockaddr,
                                ua->Address.iSockaddrLength,
                                addrstr,
                                sizeof(addrstr),
                                NULL,
                                0,
                                NI_NUMERICHOST);

                  if (ua->Address.lpSockaddr->sa_family == AF_INET) {
                     subnet_mask mask = {};
                     ::ConvertLengthToIpv4Mask(ua->OnLinkPrefixLength, (ULONG *)&mask);

                     sockaddr_in ai = *(sockaddr_in *)ua->Address.lpSockaddr;
                     ip_address address;
                     address.m_host = ::ntohl(ai.sin_addr.s_addr);
                     addresses.push_back(address);
                  }
               }
            }
         }

         free(adapter_addresses);

         return !addresses.empty();
      }

      bool get_bound_address(const udp_socket &sock, ip_endpoint &address)
      {
         if (!sock.valid()) {
            return false;
         }

         sockaddr_in addr{};
         int addrlen = sizeof(addr);
         if (::getsockname(sock.m_handle, (sockaddr *)&addr, &addrlen) == SOCKET_ERROR) {
            return false;
         }

         address.m_address.m_host = ::ntohl(addr.sin_addr.s_addr);
         address.m_port = ::ntohs(addr.sin_port);

         return true;
      }
   } // !network
} // !meteor
