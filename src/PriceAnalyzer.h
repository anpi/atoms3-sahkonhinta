#ifndef PRICE_ANALYZER_H
#define PRICE_ANALYZER_H

#include <vector>
#include "PriceData.h"

class PriceAnalyzer {
public:
  static PriceAnalysis analyzePrices(const std::vector<PriceEntry>& prices);
  
  // Exposed for testing
  static float calculate90MinAverage(const std::vector<PriceEntry>& prices, int startIdx);
  static Cheapest90Min findCheapest90MinPeriod(const std::vector<PriceEntry>& prices);
  
private:
  static int findCurrentPriceIndex(const std::vector<PriceEntry>& prices);
};

#endif
