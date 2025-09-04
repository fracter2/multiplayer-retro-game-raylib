// meteor_network.hpp

#pragma once

#include <cassert>
#include <vector>
#include <string_view>
#include <type_traits>

namespace meteor
{
   using uint64 = unsigned long long;
   using  int64 = signed long long;
   using uint32 = unsigned int;
   using  int32 = signed int;
   using uint16 = unsigned short;
   using  int16 = signed short;
   using  uint8 = unsigned char;
   using   int8 = signed char;

   namespace debug
   {
      void info(const char *format, ...);
      void warn(const char *format, ...);
      void error(const char *format, ...);
   } // !debug

   namespace time
   {
      int64 get_current_time_ms();

      template <typename T>
      constexpr T     elapsed_seconds_as(int64 duration) { return T(duration / 1000000.0); }
      constexpr float elapsed_seconds(int64 duration) { return float(duration / 1000000.0); }
      constexpr int64 elapsed_milliseconds(int64 duration) { return int64(duration / 1000.0); }
      constexpr int64 elapsed_microseconds(int64 duration) { return duration; }

      template <typename T>
      constexpr int64 from_seconds(T seconds) { return int64(seconds * 1000000.0); }

      template <typename T>
      constexpr int64 from_milliseconds(T seconds) { return int64(seconds * 1000.0); }

      template <typename T>
      constexpr int64 from_microseconds(T seconds) { return int64(seconds); }
   } // !time

   struct byte_stream {
      byte_stream() = default;

      void reset();
      bool has_data() const;
      bool can_fit(int32 bytes) const;
      int32 size() const;
      uint8 *data();
      const uint8 *data() const;

      int32 m_size = 0;
      uint8 m_data[1024] = {};
   };

   struct byte_stream_writer {
      byte_stream_writer() = delete;
      byte_stream_writer(byte_stream &stream);

      template <typename T>
      bool serialize(T value)
      {
         static_assert(std::is_fundamental_v<T>, "T needs to be a fundamental datatype!");
         if ((m_cursor + sizeof(T)) >= m_end) {
            return false;
         }

         std::memcpy(m_cursor, &value, sizeof(T));
         m_cursor += sizeof(T);
         m_stream.m_size = int32(m_cursor - m_stream.m_data);

         return true;
      }

      byte_stream &m_stream;
      uint8 *m_cursor{ nullptr };
      uint8 *m_end{ nullptr };
   };

   struct byte_stream_reader {
      byte_stream_reader() = delete;
      byte_stream_reader(byte_stream &stream);

      bool has_data() const;
      uint8 peek() const;

      template <typename T>
      bool serialize(T &value)
      {
         static_assert(std::is_fundamental_v<T>, "T needs to be a fundamental datatype!");
         if ((m_cursor + sizeof(T)) > m_end) {
            return false;
         }

         std::memcpy(&value, m_cursor, sizeof(T));
         m_cursor += sizeof(T);

         return true;
      }

      byte_stream &m_stream;
      uint8 *m_cursor = nullptr;
      uint8 *m_end = nullptr;
   };

   struct error_code {
      error_code() = default;
      error_code(int32 code);

      int32 code() const;
      const char *c_str() const;
      std::string_view string() const;
      bool is_critical() const;

      const int32 m_code = 0;
   };

   struct ip_address {
      static constexpr uint32 ANY_HOST = 0u;

      ip_address() = default;
      ip_address(uint32 host);
      ip_address(uint8 a, uint8 b, uint8 c, uint8 d);

      bool operator==(const ip_address &rhs) const;
      bool operator!=(const ip_address &rhs) const;

      void a(uint8 value);
      void b(uint8 value);
      void c(uint8 value);
      void d(uint8 value);
      uint8 a() const;
      uint8 b() const;
      uint8 c() const;
      uint8 d() const;

      uint32 m_host = ANY_HOST;
   };

   struct ip_endpoint {
      static constexpr uint16 ANY_PORT = 0u;

      ip_endpoint() = default;
      ip_endpoint(const ip_address &address, uint16 port = ANY_PORT);

      bool operator==(const ip_endpoint &rhs) const;
      bool operator!=(const ip_endpoint &rhs) const;

      ip_address &address();
      uint16 port() const;

      ip_address m_address;
      uint16     m_port = ANY_PORT;
   };

   struct udp_socket {
      udp_socket();

      bool valid() const;
      bool open();
      bool open_and_bind(const ip_endpoint &endpoint);
      void close();

      bool has_data() const;
      bool send_to(const ip_endpoint &endpoint, const byte_stream &stream);
      bool receive_from(ip_endpoint &endpoint, byte_stream &stream);

      uint64 m_handle;
   };

   namespace network
   {
      struct startup {
         startup();
         ~startup();
      };

      error_code get_last_error();
      ip_address resolve_address(std::string_view address);
      bool query_local_addresses(std::vector<ip_address> &addresses);
      bool get_bound_address(const udp_socket &socket, ip_endpoint &endpoint);
   } // !network
} // !meteor