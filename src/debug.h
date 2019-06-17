

#ifdef DEBUG
#define CR "\n"

#define DebugLog(message)                                                      \
  { Serial.print(message); }                                                   \
  while (0)

#else
#define DebugLog(message)                                                      \
  while (0) {                                                                  \
  }
#endif