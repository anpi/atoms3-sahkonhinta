#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>

// Mock Arduino String class
class String {
private:
  std::string data;
public:
  String() = default;
  String(const char* s) : data(s ? s : "") {}
  String(const std::string& s) : data(s) {}
  String(int num) : data(std::to_string(num)) {}
  
  bool operator==(const char* other) const { return data == other; }
  bool operator==(const String& other) const { return data == other.data; }
  const char* c_str() const { return data.c_str(); }
  
  // String concatenation support
  String operator+(const String& other) const {
    return String(data + other.data);
  }
  
  friend String operator+(const char* left, const String& right) {
    return String(std::string(left) + right.data);
  }
};

// Define WString_h to prevent Arduino WString.h include
#define WString_h

#include "../../src/network/IWiFiHardware.h"
#include "../../src/network/WiFiManager.h"

using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Eq;

// Mock Serial for tests
class SerialMock {
public:
  void println(const String& s) { (void)s; }
  void println(const char* s) { (void)s; }
  void printf(const char* format, ...) { (void)format; }
};
SerialMock Serial;

// Mock WiFi Hardware
class MockWiFiHardware : public IWiFiHardware {
public:
  MOCK_METHOD(int, getStatus, (), (override));
  MOCK_METHOD(void, setMode, (int mode), (override));
  MOCK_METHOD(void, disconnect, (bool wifiOff), (override));
  MOCK_METHOD(void, begin, (const char* ssid, const char* pass), (override));
  MOCK_METHOD(String, getLocalIP, (), (override));
  MOCK_METHOD(void, delayMs, (unsigned long ms), (override));
  MOCK_METHOD(void, configTime, (long gmtOffset, int daylightOffset, const char* server), (override));
};

// Test configuration constants
const char* WIFI_SSID = "TestSSID";
const char* WIFI_PASS = "TestPassword";
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 7200;
const int DAYLIGHT_OFFSET_SEC = 3600;

// WiFi status constants from WiFi.h
#define WL_CONNECTED 3
#define WL_CONNECT_FAILED 4
#define WIFI_STA 1
#define WIFI_OFF 0

#define WIFI_MANAGER_GLOBALS_DEFINED
#include "../../src/network/WiFiManager.cpp"

// Test: Already connected - should return immediately
TEST(WiFiManagerTest, ConnectWhenAlreadyConnected) {
  MockWiFiHardware mockWifi;
  WiFiManager manager(&mockWifi);

  EXPECT_CALL(mockWifi, getStatus())
    .WillOnce(Return(WL_CONNECTED));
  EXPECT_CALL(mockWifi, getLocalIP())
    .WillOnce(Return(String("192.168.1.100")));

  bool result = manager.connect();

  EXPECT_TRUE(result);
}

// Test: Successful connection on first attempt
TEST(WiFiManagerTest, ConnectSuccess) {
  MockWiFiHardware mockWifi;
  WiFiManager manager(&mockWifi);

  InSequence seq;
  
  // Initial status check - not connected
  EXPECT_CALL(mockWifi, getStatus())
    .WillOnce(Return(WL_CONNECT_FAILED));
  
  // Setup WiFi
  EXPECT_CALL(mockWifi, setMode(WIFI_STA));
  EXPECT_CALL(mockWifi, disconnect(true));
  EXPECT_CALL(mockWifi, delayMs(500));
  EXPECT_CALL(mockWifi, begin(Eq(WIFI_SSID), Eq(WIFI_PASS)));
  
  // Connection loop - connect on first iteration
  EXPECT_CALL(mockWifi, getStatus())
    .WillOnce(Return(WL_CONNECTED));
  
  // Success path
  EXPECT_CALL(mockWifi, getStatus())
    .WillOnce(Return(WL_CONNECTED));
  EXPECT_CALL(mockWifi, getLocalIP())
    .WillOnce(Return(String("192.168.1.50")));
  EXPECT_CALL(mockWifi, configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, Eq(NTP_SERVER)));

  bool result = manager.connect();

  EXPECT_TRUE(result);
}

// Test: Connection after multiple retries
TEST(WiFiManagerTest, ConnectAfterRetries) {
  MockWiFiHardware mockWifi;
  WiFiManager manager(&mockWifi);

  // Don't use InSequence for this test - just use Times()
  
  // Initial status check
  EXPECT_CALL(mockWifi, getStatus())
    .Times(7)  // Initial + 3 loop checks + final check + check for printing + success check
    .WillOnce(Return(WL_CONNECT_FAILED))   // Initial
    .WillOnce(Return(WL_CONNECT_FAILED))   // Loop iteration 0
    .WillOnce(Return(WL_CONNECT_FAILED))   // Status check for print (i=0)
    .WillOnce(Return(WL_CONNECT_FAILED))   // Loop iteration 1
    .WillOnce(Return(WL_CONNECT_FAILED))   // Loop iteration 2
    .WillOnce(Return(WL_CONNECTED))        // Loop check - breaks
    .WillOnce(Return(WL_CONNECTED));       // Final success check
  
  // Setup
  EXPECT_CALL(mockWifi, setMode(WIFI_STA));
  EXPECT_CALL(mockWifi, disconnect(true));
  EXPECT_CALL(mockWifi, delayMs(500))
    .Times(4);  // Once for setup, 3 times in loop
  EXPECT_CALL(mockWifi, begin(Eq(WIFI_SSID), Eq(WIFI_PASS)));
  
  // Success path
  EXPECT_CALL(mockWifi, getLocalIP())
    .WillOnce(Return(String("192.168.1.75")));
  EXPECT_CALL(mockWifi, configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, Eq(NTP_SERVER)));

  bool result = manager.connect();

  EXPECT_TRUE(result);
}

// Test: Connection timeout after 40 retries
TEST(WiFiManagerTest, ConnectTimeout) {
  MockWiFiHardware mockWifi;
  WiFiManager manager(&mockWifi);

  InSequence seq;
  
  // Initial status check
  EXPECT_CALL(mockWifi, getStatus())
    .WillOnce(Return(WL_CONNECT_FAILED));
  
  // Setup
  EXPECT_CALL(mockWifi, setMode(WIFI_STA));
  EXPECT_CALL(mockWifi, disconnect(true));
  EXPECT_CALL(mockWifi, delayMs(500));
  EXPECT_CALL(mockWifi, begin(Eq(WIFI_SSID), Eq(WIFI_PASS)));
  
  // Connection loop - fail all 40 attempts
  for (int i = 0; i < 40; ++i) {
    EXPECT_CALL(mockWifi, getStatus())
      .WillOnce(Return(WL_CONNECT_FAILED));
    EXPECT_CALL(mockWifi, delayMs(500));
    
    // Status check every 5 iterations
    if (i % 5 == 0) {
      EXPECT_CALL(mockWifi, getStatus())
        .WillOnce(Return(WL_CONNECT_FAILED));
    }
  }
  
  // Final status check - still failed
  EXPECT_CALL(mockWifi, getStatus())
    .WillOnce(Return(WL_CONNECT_FAILED));
  EXPECT_CALL(mockWifi, getStatus())
    .WillOnce(Return(WL_CONNECT_FAILED));

  bool result = manager.connect();

  EXPECT_FALSE(result);
}

// Test: Disconnect when connected
TEST(WiFiManagerTest, DisconnectWhenConnected) {
  MockWiFiHardware mockWifi;
  WiFiManager manager(&mockWifi);

  InSequence seq;
  
  EXPECT_CALL(mockWifi, getStatus())
    .WillOnce(Return(WL_CONNECTED));
  EXPECT_CALL(mockWifi, disconnect(true));
  EXPECT_CALL(mockWifi, setMode(WIFI_OFF));

  manager.disconnect();
}

// Test: Disconnect when not connected
TEST(WiFiManagerTest, DisconnectWhenNotConnected) {
  MockWiFiHardware mockWifi;
  WiFiManager manager(&mockWifi);

  EXPECT_CALL(mockWifi, getStatus())
    .WillOnce(Return(WL_CONNECT_FAILED));

  manager.disconnect();
}

// Test: isConnected returns true when connected
TEST(WiFiManagerTest, IsConnectedTrue) {
  MockWiFiHardware mockWifi;
  WiFiManager manager(&mockWifi);

  EXPECT_CALL(mockWifi, getStatus())
    .WillOnce(Return(WL_CONNECTED));

  EXPECT_TRUE(manager.isConnected());
}

// Test: isConnected returns false when not connected
TEST(WiFiManagerTest, IsConnectedFalse) {
  MockWiFiHardware mockWifi;
  WiFiManager manager(&mockWifi);

  EXPECT_CALL(mockWifi, getStatus())
    .WillOnce(Return(WL_CONNECT_FAILED));

  EXPECT_FALSE(manager.isConnected());
}

// Test: getIP returns IP address
TEST(WiFiManagerTest, GetIP) {
  MockWiFiHardware mockWifi;
  WiFiManager manager(&mockWifi);

  EXPECT_CALL(mockWifi, getLocalIP())
    .WillOnce(Return(String("10.0.0.123")));

  String ip = manager.getIP();

  EXPECT_EQ(ip, "10.0.0.123");
}
