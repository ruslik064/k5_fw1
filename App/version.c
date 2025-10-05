#include "version.h"
#include <assert.h>

#ifndef VERSION_STRING
#define VERSION_STRING "1.01.00"
#endif

// const char Version[] = "OEFW-" VERSION_STRING;
// const char UART_Version[45] = "UV-K5 Firmware, Open Edition, OEFW-" VERSION_STRING "\r\n";

const char Version[] = "muzkr " VERSION_STRING;
const char UART_Version[28] = VERSION_STRING "\0(c) 2025 muzkr\r\n";

static_assert(sizeof(Version) <= 16, "Version[] overflow");
