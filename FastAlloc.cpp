#include "FastAlloc.h"
#include <iostream>

int main() {
  std::cout << "=== FastAlloc Library Demo ===\n\n";

  // 1. Show that AI is already trained
  // We expect to see >100 allocations in stats immediately
  std::cout << "Checking AI status immediately after startup...\n";
  // Using a hacked way to access the private member or just run stats if
  // available Since g_FastAlloc is static in the header, we can call
  // debug_stats via the helper if we exposed one, or just trust the system is
  // working. Ideally we'd add `FastAllocStats()` to the API. Let's rely on the
  // internal print at the end to prove it.

  // 2. Normal Usage
  std::cout << "Allocating user array...\n";
  int *arr = (int *)FastAlloc(10 * sizeof(int));
  for (int i = 0; i < 10; ++i)
    arr[i] = i * 100;

  std::cout << "Data: ";
  for (int i = 0; i < 10; ++i)
    std::cout << arr[i] << " ";
  std::cout << "\n";
  FastFree(arr);

  // 3. Verify Stats
  // The verify step: we expect '24 bytes (120 times)' or similar from the
  // pre-training.
  std::cout << "\nFinal System Stats:\n";
  // We need to expose the stats function via a C-style wrapper or just call the
  // object method if we modified the header to allow access. For this simple
  // demo, I'll access the static object directly if I can, but since it's
  // static in header, it is visible here.
  g_FastAlloc.debug_stats();

  return 0;
}
