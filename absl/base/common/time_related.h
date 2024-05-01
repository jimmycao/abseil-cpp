#pragma once

#include <time.h>

namespace mlf {

// Get time in seconds.
inline double current_realtime() { 
    struct timespec tp; 
    clock_gettime(CLOCK_REALTIME, &tp); 
    return tp.tv_sec + tp.tv_nsec * 1e-9;
}

inline uint64_t current_realtime_s() { 
    struct timespec tp; 
    clock_gettime(CLOCK_REALTIME, &tp); 
    return tp.tv_sec;
}

} //namespace mlf
