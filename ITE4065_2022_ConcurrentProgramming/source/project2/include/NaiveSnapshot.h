#include "ISnapshot.h"
#include "StampedValue.h"
#include "GC.h"

#define GC_LIMIT_NAIVE_SNAP_SHOT 50000

class NaiveSnapshot : ISnapshot {
  private:
    // MRSW register array
    StampedValue **a_table;
    // GC mechanism
    GC<StampedValue> *gc;

  public:
    // number of threads
    int capacity;

    NaiveSnapshot(int _capacity);
    ~NaiveSnapshot();

    // Updates tid's register with value
    void update(int value, int tid);
    // collect integer value from register array
    int *collect();
    // Get snapshot of register array (In Naive, simply call collect function)
    int *scan();
};
