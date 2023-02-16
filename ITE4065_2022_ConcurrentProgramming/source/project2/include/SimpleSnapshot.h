#include "ISnapshot.h"
#include "StampedValue.h"
#include "GC.h"

#define GC_LIMIT_SIMPLE_SNAPSHOT 50000

class SimpleSnapshot : ISnapshot {
  private:
    // MRSW register array
    StampedValue **a_table;
    // GC mechanism
    GC<StampedValue> *gc;

  public:
    // number of threads
    int capacity;

    SimpleSnapshot(int _capacity);
    ~SimpleSnapshot();

    // Updates tid's register with value
    void update(int value, int tid);
    // collect StampedValue value from register array
    StampedValue **collect();
    // Get atomic snapshot of register array (In Naive, simply call collect function)
    int *scan();
};
