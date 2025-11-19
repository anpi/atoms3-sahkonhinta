// Mock Arduino types for testing without Arduino SDK
#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#include <string>
#include <cstdint>

// Mock String class
class String {
private:
  std::string data;
public:
  String() = default;
  String(const char* s) : data(s ? s : "") {}
  String(const std::string& s) : data(s) {}
  String(int) : data() {}  // Mock constructor
  
  size_t length() const { return data.length(); }
  const char* c_str() const { return data.c_str(); }
  
  String& operator=(const char* s) { data = s ? s : ""; return *this; }
  String& operator=(const std::string& s) { data = s; return *this; }
};

#endif // MOCK_ARDUINO_H
