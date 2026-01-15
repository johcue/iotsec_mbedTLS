#include <Arduino.h>

extern "C" void wolfSSL_Arduino_Serial_Print(const char* msg)
{
    // Logging intentionally disabled.
    // Required to satisfy Arduino-wolfSSL linker dependency.
    (void)msg;
}
