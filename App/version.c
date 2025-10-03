#include "version.h"

#ifndef VERSION_STRING
#define VERSION_STRING "1.01.00"
#endif

const char Version[] = "muzkr " VERSION_STRING;
const char UART_Version[] = VERSION_STRING "\0(c) 2025 muzkr\r\n";
