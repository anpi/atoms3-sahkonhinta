#include "App.h"
#include <M5AtomS3.h>

App::App() : priceMonitor(&displayManager, &apiClient) {}

void App::setup() {
  auto cfg = M5.config();
  AtomS3.begin(cfg);
  setCpuFrequencyMhz(80);

  Serial.begin(115200);
  
  pinMode(GPIO_NUM_41, INPUT_PULLUP);
  attachInterrupt(GPIO_NUM_41, buttonISR, FALLING);
  
  timerManager.setup();

  displayManager.initialize();
  displayManager.showText("Connecting...", WIFI_SSID);
  bool connected = wifiManager.connect();
  
  if (connected) {
    displayManager.showText("WiFi OK", wifiManager.getIP());
    delay(1500);
    
    Serial.println("Fetching initial prices...");
    bool success = priceMonitor.fetchAndAnalyzePrices();
    if (success) {
      displayManager.showAnalysis(priceMonitor.getLastAnalysis());
    }
    
    wifiManager.disconnect();
    setCpuFrequencyMhz(10);
    Serial.println("CPU reduced to 10 MHz for idle");
    timerManager.scheduleNextUpdate();
  } else {
    displayManager.showText("WiFi FAILED", "Retrying...");
    delay(2000);
  }
}

void App::loop() {
  AtomS3.update();
  
  if (buttonWakeFlag) {
    buttonWakeFlag = false;
    handleButtonPress();
  }
  
  if (timerManager.wasTriggered()) {
    handleScheduledUpdate();
    timerManager.scheduleNextUpdate();
  }
  
  if (displayManager.isBright()) {
    displayManager.updateBrightness(priceMonitor.isFetchingPrice());
    delay(50);
  } else {
    idleManager.waitForInterrupt();
  }
}

bool App::fetchPriceWithWifi() {
  setCpuFrequencyMhz(80);
  Serial.println("CPU boosted to 80 MHz for WiFi");
  
  bool wasConnected = wifiManager.isConnected();
  
  if (!wasConnected) {
    Serial.println("Connecting WiFi for price fetch...");
    if (priceMonitor.getLastAnalysis().valid) {
      displayManager.showWifiIndicator();
    }
    bool connected = wifiManager.connect();
    if (!connected) {
      displayManager.showText("WiFi FAILED");
      setCpuFrequencyMhz(10);
      Serial.println("CPU reduced to 10 MHz after WiFi failure");
      return false;
    }
  }
  
  bool success = priceMonitor.fetchAndAnalyzePrices();
  
  wifiManager.disconnect();
  setCpuFrequencyMhz(10);
  Serial.println("CPU reduced to 10 MHz for idle");
  
  return success;
}

void App::handleButtonPress() {
  Serial.println("Button pressed, fetching prices...");
  displayManager.setBrightness(true);
  
  bool success = fetchPriceWithWifi();
  if (success) {
    displayManager.showAnalysis(priceMonitor.getLastAnalysis());
  } else if (priceMonitor.getLastAnalysis().valid) {
    displayManager.showAnalysis(priceMonitor.getLastAnalysis());
  }
  
  displayManager.setBrightUntil(millis() + 5000);
}

void App::handleScheduledUpdate() {
  if (priceMonitor.isScheduledUpdateTime()) {
    bool success = fetchPriceWithWifi();
    if (success) {
      displayManager.showAnalysis(priceMonitor.getLastAnalysis());
    } else if (priceMonitor.getLastAnalysis().valid) {
      displayManager.showAnalysis(priceMonitor.getLastAnalysis());
    }
  }
}
