#ifndef PRICE_DATA_H
#define PRICE_DATA_H

#ifndef WString_h
  #include <WString.h>
#endif

struct PriceEntry {
  String dateTime;
  float priceWithTax;
};

struct Cheapest90Min {
  float avg;
  int startIndex;  // Index in prices array where cheapest period starts
  
  Cheapest90Min() : avg(-1), startIndex(-1) {}
};

struct PriceAnalysis {
  float next90MinAvg;
  float cheapest90MinAvg;
  String cheapest90MinTime;
  String currentPeriodStartTime;          // Current period start time (HH:MM)
  String lastFetchTime;       // When data was last fetched (HH:MM)
  bool cheapestIsTomorrow;
  bool valid;
  
  PriceAnalysis() : next90MinAvg(-1), cheapest90MinAvg(-1), cheapestIsTomorrow(false), valid(false) {}
};

#endif
