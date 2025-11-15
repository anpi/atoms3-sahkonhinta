#ifndef MOCK_STRING_H
#define MOCK_STRING_H

#include <string>
#include <cstring>

// Minimal Arduino String implementation for native testing
class String {
private:
  std::string data;

public:
  String() : data("") {}
  String(const char* str) : data(str ? str : "") {}
  String(const std::string& str) : data(str) {}
  String(const String& other) = default;
  String(int value) : data(std::to_string(value)) {}
  String(float value) : data(std::to_string(value)) {}
  
  const char* c_str() const { return data.c_str(); }
  size_t length() const { return data.length(); }
  
  String substring(int start) const {
    if (start >= (int)data.length()) return String("");
    return String(data.substr(start));
  }
  
  String substring(int start, int end) const {
    if (start >= (int)data.length()) return String("");
    if (end > (int)data.length()) end = data.length();
    if (start >= end) return String("");
    return String(data.substr(start, end - start));
  }
  
  bool startsWith(const String& prefix) const {
    return data.find(prefix.data) == 0;
  }
  
  bool operator==(const String& other) const {
    return data == other.data;
  }
  
  bool operator==(const char* other) const {
    return data == other;
  }
  
  String& operator=(const char* str) {
    data = str ? str : "";
    return *this;
  }
  
  String& operator=(const String& other) {
    data = other.data;
    return *this;
  }
};

#endif
