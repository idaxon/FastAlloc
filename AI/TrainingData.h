#ifndef TRAINING_DATA_H
#define TRAINING_DATA_H

#include <vector>

// Simulated "Industry Level" usage log
// Distribution:
// - 32 bytes:  ~50% (High freq)
// - 64 bytes:  ~30%
// - 128 bytes: ~15%
// - 256 bytes: ~5%

namespace FastAllocAI {

const std::vector<int> training_dataset = {
    // We generate a pattern that repeats to simulate 5000+ entries
    // Batch 1
    32, 32, 32, 32, 32, 64, 64, 64, 128, 256, 32, 32, 32, 64, 128, 32, 32, 64,
    32, 32,
    // Batch 2 (Slightly varied)
    32, 64, 32, 32, 128, 32, 64, 32, 256, 32, 32, 32, 64, 64, 128, 32, 32, 32,
    32, 64,
    // ... Ideally this file would be huge, but for compilation speed
    // and demonstration we will rely on the Predictor to repeat this
    // pattern 100 times to simulate 4000 datapoints.
};

// Helper to get total count simulated
const int SIMULATION_MULTIPLIER = 100;
} // namespace FastAllocAI

#endif
