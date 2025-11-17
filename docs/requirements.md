# Requirements Specification: AtomS3 Electricity Price Monitor

**Version:** 1.0  
**Last Updated:** 17 November 2025  
**Target Hardware:** M5Stack AtomS3  
**Test Coverage:** 27 unit tests

---

## Functional Requirements

### FR-001: Price Data Acquisition

The system SHALL fetch electricity price data from the Spot-hinta.fi API.

**Acceptance Criteria:**
- Data fetched automatically every 15 minutes at :00, :15, :30, :45
- Manual fetch triggered by button press
- Failed fetches don't crash the system

---

### FR-002: Current Period Price Display

The system SHALL display the next 90-minute average electricity price.

**Acceptance Criteria:**
- Display shows "Nyt HH:MM" label indicating current 15-minute period start
- Background color indicates price level: Green < 8¢, Yellow 8-15¢, Red > 15¢
- "N/A" displayed when insufficient future data

---

### FR-003: Cheapest Period Calculation

The system SHALL identify the cheapest 90-minute period for running appliances.

**Acceptance Criteria:**
- Period must start at or after 7:00 and end at or before 23:00
- Periods crossing midnight are rejected
- Tomorrow indicator "(huo)" shown when cheapest period is next day
- Returns invalid when no valid period exists

**Test Coverage:** `test_price_analyzer_basic.cpp`, `test_price_analyzer_time_constraints.cpp`

---

### FR-004: Time Period Analysis

The system SHALL calculate 90-minute rolling averages from 15-minute price data.

**Acceptance Criteria:**
- 90-minute period = 6 consecutive 15-minute intervals
- Returns -1 when insufficient data available

**Test Coverage:** `test_price_analyzer_basic.cpp`

---

### FR-005: Data Timestamp Display

The system SHALL display when price data was last fetched.

**Acceptance Criteria:**
- Display shows "Päivitetty HH:MM" at bottom
- Timestamp captured at moment of successful API fetch
- Timestamp separate from current period start time

---

### FR-006: Button Interaction

The system SHALL respond to button press for manual updates.

**Acceptance Criteria:**
- Single press triggers immediate price fetch
- Display brightness increased for 5 seconds

---

### FR-007: Display Brightness Management

The system SHALL manage display brightness for power efficiency.

**Acceptance Criteria:**
- Default brightness: 1/255 (minimal)
- Bright mode (255/255) active for 5 seconds after button press or scheduled update
- Automatic return to dim mode after timeout

---

## Non-Functional Requirements

### NFR-001: Performance

**Requirements:**
- Price analysis completes within 100ms
- No blocking operations in main loop
- Responsive to button press within 100ms

**Metrics:**
- Unit test suite runs in < 1 second
- Binary size: ~1.26 MB (37% of available flash)

---

### NFR-002: Reliability

**Requirements:**
- System recovers gracefully from network failures
- Invalid API responses don't crash device
- Maintains last valid data on fetch failure

**Test Coverage:** `test_price_analyzer_edge_cases.cpp`, `test_price_analyzer_datetime.cpp`

---

### NFR-003: Usability

**Requirements:**
- Color coding provides instant price assessment
- Finnish language labels for local users
- No abbreviations (full "Päivitetty" text)
- High contrast text on colored backgrounds

---

## Technical Constraints

### TC-001: Hardware Platform

**Constraints:**
- Device: M5Stack AtomS3 (ESP32-S3)
- Display: 128x128 TFT LCD
- Flash: 8MB (using ~1.26MB / 37%)
- Single button input

---

### TC-002: External Dependencies

**Dependencies:**
- M5AtomS3 library
- ArduinoJson library (v6+)
- NTP time synchronization
- Spot-hinta.fi API availability

**API Endpoint:**
```
GET https://api.spot-hinta.fi/TodayAndDayForward?region=FI
Response: JSON array of price entries
[
  {
    "DateTime": "2025-11-17T00:00:00+02:00",
    "PriceWithTax": 0.0845
  },
  ...
]
```

---

### TC-003: Time Constraints

**Constraints:**
- Cheapest period search limited to 7:00-23:00 (washing machines cannot run during night)
- 90-minute period requirement (typical washing machine cycle duration)
- 15-minute data granularity (API limitation)