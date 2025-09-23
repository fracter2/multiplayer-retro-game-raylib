// timer.cpp

#include "timer.hpp"
#include "raylib.h"

namespace meteor
{
   void timer::reset()
   {
      m_start = 0.0;
      m_end = 0.0;
   }

   void timer::start()
   {
      m_start = m_end = GetTime();
   }

   void timer::stop()
   {
      m_end = GetTime();
   }

   double timer::duration() const
   {
      return m_end - m_start;
   }
} // !meteor
