#ifndef AI_PREDICTOR_H
#define AI_PREDICTOR_H

#include "TrainingData.h"
#include <iostream>
#include <map>
#include <vector>

namespace FastAllocAI {

class AIPredictor {
private:
  std::map<size_t, int> allocationCounts;
  size_t mostFrequentSize = 0;
  int maxCount = 0;
  bool isTrained = false;

public:
  AIPredictor() {
    // Auto-train on instantiation
    train();
  }

  void train() {
    if (isTrained)
      return;

    // Load data from TrainingData.h
    // We simulate a massive dataset by repeating the pattern
    for (int m = 0; m < SIMULATION_MULTIPLIER; ++m) {
      for (int size : training_dataset) {
        recordAllocation(static_cast<size_t>(size));
      }
    }
    isTrained = true;
    // std::cout << "[AI] Training complete. Processed "
    //           << (training_dataset.size() * SIMULATION_MULTIPLIER)
    //           << " records.\n";
  }

  void recordAllocation(size_t size) {
    allocationCounts[size]++;
    if (allocationCounts[size] > maxCount) {
      maxCount = allocationCounts[size];
      mostFrequentSize = size;
    }
  }

  size_t getSuggestedPoolSize() const { return mostFrequentSize; }

  void printStats() const {
    std::cout << "[AI Predictor] Most frequent allocation size: "
              << mostFrequentSize << " bytes (" << maxCount << " times)\n";
  }
};

} // namespace FastAllocAI

#endif
