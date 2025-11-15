// String adapter for tests - wraps std::string with Arduino String API
#ifndef TEST_STRING_ADAPTER_H
#define TEST_STRING_ADAPTER_H

#include <string>
#include <cstring>

class String {
private:
  std::string data;

public:
  String() : data() {}
  String(const char* str) : data(str ? str : "") {}
  String(const std::string& str) : data(str) {}
  String(const String& other) : data(other.data) {}
  
  String& operator=(const String& other) {
    data = other.data;
    return *this;
  }
  
  String& operator=(const char* str) {
    data = str ? str : "";
    return *this;
  }
  
  // Arduino String API
  String substring(size_t from, size_t to) const {
    if (from >= data.length()) return String();
    size_t len = (to > from) ? (to - from) : 0;
    return String(data.substr(from, len));
  }
  
  bool startsWith(const String& prefix) const {
    return data.rfind(prefix.data, 0) == 0;
  }
  
  size_t length() const {
    return data.length();
  }
  
  const char* c_str() const {
    return data.c_str();
  }
  
  bool operator==(const String& other) const {
    return data == other.data;
  }
  
  bool operator==(const char* str) const {
    return data == (str ? str : "");
  }
  
  // For std::cout
  friend std::ostream& operator<<(std::ostream& os, const String& str) {
    os << str.data;
    return os;
  }
};

#endif // TEST_STRING_ADAPTER_H
