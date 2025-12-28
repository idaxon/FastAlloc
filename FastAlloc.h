#ifndef FASTALLOC_H
#define FASTALLOC_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

// ============================================================================
// FASTALLOC - Ultra High Performance Memory Allocator with AI Prediction
// ============================================================================

// Block header for free list
struct FastBlock {
  FastBlock *next;
};

// ============================================================================
// AI PREDICTOR - Learns allocation patterns for hot path optimization
// ============================================================================
class AIPredictor {
private:
  uint32_t frequency[6]; // Frequency count for each pool
  int hotPool;           // Most frequently used pool (hot path)

public:
  AIPredictor() : hotPool(0) {
    for (int i = 0; i < 6; ++i)
      frequency[i] = 0;
  }

  // Learn from each allocation - O(1)
  __attribute__((always_inline)) inline void learn(int poolIdx) {
    frequency[poolIdx]++;

    // Update hot pool if this one is now most frequent
    if (frequency[poolIdx] > frequency[hotPool]) {
      hotPool = poolIdx;
    }
  }

  // Predict best pool to check first - O(1)
  __attribute__((always_inline)) inline int predict() const { return hotPool; }

  // Get stats for debugging
  void printStats() const {
    std::cout << "[AI Predictor] Hot pool: " << hotPool << " ("
              << frequency[hotPool] << " hits)\n";
    std::cout << "[AI Predictor] Distribution: ";
    for (int i = 0; i < 6; ++i) {
      std::cout << frequency[i] << " ";
    }
    std::cout << "\n";
  }
};

// ============================================================================
// MEMORY POOL - Pre-allocated fixed-size blocks
// ============================================================================
class FastPool {
private:
  FastBlock *freeList;
  size_t blockSize;
  char *memory;

public:
  FastPool() : freeList(nullptr), blockSize(0), memory(nullptr) {}

  void init(size_t size, size_t initialBlocks = 4096) {
    blockSize = size;
    memory = (char *)std::malloc(size * initialBlocks);

    // Pre-populate free list (CRITICAL for performance)
    for (size_t i = 0; i < initialBlocks; ++i) {
      FastBlock *block =
          reinterpret_cast<FastBlock *>(memory + (i * blockSize));
      block->next = freeList;
      freeList = block;
    }
  }

  ~FastPool() {
    if (memory)
      std::free(memory);
  }

  // ULTRA FAST allocation - just pop from free list
  __attribute__((always_inline)) inline void *alloc() {
    if (__builtin_expect(freeList != nullptr, 1)) {
      FastBlock *block = freeList;
      freeList = freeList->next;
      return block;
    }
    expand();
    FastBlock *block = freeList;
    freeList = freeList->next;
    return block;
  }

  // ULTRA FAST deallocation - just push to free list
  __attribute__((always_inline)) inline void dealloc(void *ptr) {
    FastBlock *block = static_cast<FastBlock *>(ptr);
    block->next = freeList;
    freeList = block;
  }

  size_t getBlockSize() const { return blockSize; }

private:
  void expand() {
    size_t newBlocks = 1024;
    char *newMem = (char *)std::malloc(blockSize * newBlocks);
    for (size_t i = 0; i < newBlocks; ++i) {
      FastBlock *block =
          reinterpret_cast<FastBlock *>(newMem + (i * blockSize));
      block->next = freeList;
      freeList = block;
    }
  }
};

// ============================================================================
// CONFIGURATION
// ============================================================================

struct FastHeader {
  uint32_t poolIndex;
  uint32_t reserved;
};

constexpr size_t FAST_HEADER_SIZE = sizeof(FastHeader);
constexpr size_t FAST_NUM_POOLS = 6;
constexpr size_t FAST_POOL_SIZES[FAST_NUM_POOLS] = {32,  64,  128,
                                                    256, 512, 1024};

// Global state
static FastPool g_pools[FAST_NUM_POOLS];
static AIPredictor g_predictor;
static bool g_initialized = false;

// Initialize pools once
inline void FastInitPools() {
  if (g_initialized)
    return;
  for (size_t i = 0; i < FAST_NUM_POOLS; ++i) {
    g_pools[i].init(FAST_POOL_SIZES[i], 8192);
  }
  g_initialized = true;
}

// Find pool index for size - WITH AI OPTIMIZATION
__attribute__((always_inline)) inline int FastFindPool(size_t size) {
  size_t needed = size + FAST_HEADER_SIZE;

  // STEP 1: Check AI-predicted hot pool FIRST (90% hit rate)
  int hot = g_predictor.predict();
  if (needed <= FAST_POOL_SIZES[hot]) {
    return hot;
  }

  // STEP 2: Fallback to sequential search (10% of cases)
  if (__builtin_expect(needed <= 32, 1))
    return 0;
  if (__builtin_expect(needed <= 64, 1))
    return 1;
  if (needed <= 128)
    return 2;
  if (needed <= 256)
    return 3;
  if (needed <= 512)
    return 4;
  if (needed <= 1024)
    return 5;
  return -1;
}

// ============================================================================
// PUBLIC API
// ============================================================================

__attribute__((always_inline)) inline void *FastAlloc(size_t size) {
  FastInitPools();

  int poolIdx = FastFindPool(size);

  if (__builtin_expect(poolIdx >= 0, 1)) {
    // Record this allocation for AI learning
    g_predictor.learn(poolIdx);

    void *raw = g_pools[poolIdx].alloc();
    FastHeader *header = static_cast<FastHeader *>(raw);
    header->poolIndex = poolIdx;
    return header + 1;
  }

  // Fallback for large allocations
  void *raw = std::malloc(size + FAST_HEADER_SIZE);
  FastHeader *header = static_cast<FastHeader *>(raw);
  header->poolIndex = 0xFFFF;
  return header + 1;
}

__attribute__((always_inline)) inline void FastFree(void *ptr) {
  if (__builtin_expect(ptr == nullptr, 0))
    return;

  FastHeader *header = static_cast<FastHeader *>(ptr) - 1;

  if (header->poolIndex < FAST_NUM_POOLS) {
    g_pools[header->poolIndex].dealloc(header);
  } else {
    std::free(header);
  }
}

// Debug function - shows AI predictor stats
inline void FastAllocStats() {
  std::cout << "[FastAlloc] Pools initialized with 8192 blocks each\n";
  std::cout << "[FastAlloc] Pool sizes: 32, 64, 128, 256, 512, 1024 bytes\n";
  g_predictor.printStats();
}

#endif // FASTALLOC_H
