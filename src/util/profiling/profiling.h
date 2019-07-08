#include "util/profiling/timescope.h"

#ifdef PROFILE
  #define TIME_SCOPE_OBJECT(name, fps)  Profiling::TimeScope ts(name, fps);
  #define TIME_SCOPE(name)              TIME_SCOPE_OBJECT(name, false)
  #define TIME_SCOPE_FPS(name)          TIME_SCOPE_OBJECT(name, true)
  #define TIME_SCOPE_SECTION_START(m)   ts.section_start(m);
  #define TIME_SCOPE_SECTION_END()      ts.section_end();
#else
  #define TIME_SCOPE_OBJECT(name, fps)
  #define TIME_SCOPE(name)
  #define TIME_SCOPE_FPS(name)
  #define TIME_SCOPE_SECTION_START(m)
  #define TIME_SCOPE_SECTION_END()
#endif
