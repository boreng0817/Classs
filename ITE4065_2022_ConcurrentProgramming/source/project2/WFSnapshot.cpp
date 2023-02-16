#include "WFSnapshot.h"
#include <memory>


// Constructor of WFSnapshot
WFSnapshot::WFSnapshot(int _capacity) {
  capacity = _capacity;
  a_table = new WFStampedValue*[capacity];

  for (int i = 0; i < capacity; ++i) {
    a_table[i] = new WFStampedValue(0, capacity);
  }

}

// Destructor of WFSnapshot
WFSnapshot::~WFSnapshot() {
  for (int i = 0; i < capacity; ++i) {
    delete a_table[i];
  }

  delete[] a_table;
}

void WFSnapshot::update(int value, int tid) {

  // Before update, perform scan operation.
  // In SimpleSnapshot, scan returns atomic snapshot.
  int *snap = scan();

  // Update new value for [tid]thread register.
  WFStampedValue *oldValue = a_table[tid];
  WFStampedValue *newValue = new WFStampedValue(oldValue->stamp + 1, value, snap);
  a_table[tid] = newValue;

  // We can't free snap array. It can be used later for scan operation.
}

WFStampedValue **WFSnapshot::collect() {
  WFStampedValue **copy = new WFStampedValue*[capacity];

  // Collect WFStampedValue from register array.
  for (int i = 0; i < capacity; ++i) {
    copy[i] = a_table[i];
  }
  return copy;
}

int *WFSnapshot::scan() {
  WFStampedValue **oldCopy, **newCopy;
  // moved value to check if single thread updated twice
  std::unique_ptr<bool[]> moved(new bool[capacity]);

  // Initialise moved array with false
  for (int i = 0; i < capacity; ++i) {
    moved[i] = false;
  }

  // collect once
  oldCopy = collect();
  int i;
  while (true) {
    // collect once more
    newCopy = collect();
    for (i = 0; i < capacity; ++i) {
      // Check if thread i had updated.
      if (oldCopy[i]->stamp != newCopy[i]->stamp) {
        // If thread moved twice, use newCopy's snap.
        if (moved[i]) {
          int *ret = new int[capacity];
          for (int j = 0; j < capacity; ++j) {
            ret[j] = newCopy[i]->snap[j];
          }

          delete[] newCopy;
          delete[] oldCopy;

          return ret;
        // Else if this thread is first time updated, try again.
        } else {
          moved[i] = true;
          delete[] oldCopy;
          oldCopy = newCopy;
          break;
        }
      }
    }
    // GOTO collect if oldCopy != newCopy.
    if (i == capacity) {
      break;
    }
  }


  // Make integer snapshot.
  int *result = new int[capacity];
  for (int i = 0; i < capacity; ++i) {
    result[i] = newCopy[i]->value;
  }

  // Free unused array.
  delete[] newCopy;
  delete[] oldCopy;
  return result;
}
