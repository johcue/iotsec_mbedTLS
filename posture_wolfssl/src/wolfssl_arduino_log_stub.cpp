extern "C" {

// This satisfies the linker when wolfSSL logging is compiled in.
// We intentionally do NOTHING here.
void wolfSSL_Arduino_Serial_Print(const char* msg) {
  (void)msg;
}

}
