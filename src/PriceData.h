#ifndef PRICE_DATA_H
#define PRICE_DATA_H

#ifndef WString_h
  #include <WString.h>
#endif

struct PriceEntry {
  String dateTime;
  float priceWithTax;
  int rank;
};

struct Cheapest90Min {
  float avg;
  int startIndex;  // Index in prices array where cheapest period starts
  
  Cheapest90Min() : avg(-1), startIndex(-1) {}
};

struct PriceAnalysis {
  float currentPrice;
  float next90MinAvg;
  float cheapest90MinAvg;
  String cheapest90MinTime;
  String updateTime;
  bool valid;
  
  PriceAnalysis() : currentPrice(-1), next90MinAvg(-1), cheapest90MinAvg(-1), valid(false) {}
};

#endif
