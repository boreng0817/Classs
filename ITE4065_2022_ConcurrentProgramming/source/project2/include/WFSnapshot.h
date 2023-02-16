#include "ISnapshot.h"
#include "GC.h"

#define GC_LIMIT_WF_SNAPSHOT 50000

// Stamped value with snapshot.
class WFStampedValue {
  public:
    int value;
    int stamp;
    int *snap;

    WFStampedValue() {
    }
    WFStampedValue(int _value, int size) {
      value = _value;
      stamp = 0;
      snap = new int[size];
    }
    WFStampedValue(long _label, int _value, int *_snap) {
      value = _value;
      stamp = _label;
      snap = _snap;
    }
    ~WFStampedValue() {
      delete[] snap;
    }
};

class WFSnapshot : ISnapshot {
  private:
    // MRSW register array
    WFStampedValue **a_table;
    // GC mechanism
    GC<WFStampedValue> *gc;

  public:
    // number of threads
    int capacity;

    WFSnapshot(int _capacity);
    ~WFSnapshot();

    // Updates tid's register with value
    void update(int value, int tid);
    // collect StampedValue value from register array
    WFStampedValue **collect();
    // Get atomic snapshot of register array (In Naive, simply call collect function)
    int *scan();
};
