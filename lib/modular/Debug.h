#ifndef debug_h
#define debug_h

#ifdef DEBUG
#define debug(...) Serial.print(__VA_ARGS__)
#else
#define debug(...)
#endif

#ifdef DEBUG
#define debugLn(...); Serial.println(__VA_ARGS__)
#else
#define debugLn(...)
#endif

#endif

