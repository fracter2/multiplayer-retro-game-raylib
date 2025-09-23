// timer.hpp

#pragma once

namespace meteor
{
   struct timer {
      timer() = default;

      void reset();
      void start();
      void stop();
      double duration() const;

      double m_start = 0;
      double m_end = 0;
   };
} // !meteor
