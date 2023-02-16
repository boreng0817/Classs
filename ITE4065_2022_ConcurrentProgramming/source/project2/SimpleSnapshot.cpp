#include "SimpleSnapshot.h"


// Constructor of SimpleSnapshot
SimpleSnapshot::SimpleSnapshot(int _capacity) {
  capacity = _capacity;
  a_table = new StampedValue*[capacity];

  for (int i = 0; i < capacity; ++i) {
    a_table[i] = new StampedValue(0);
  }

}

// Destructor of SimpleSnapshot
SimpleSnapshot::~SimpleSnapshot() {
  for (int i = 0; i < capacity; ++i) {
    delete a_table[i];
  }

  delete[] a_table;
}

void SimpleSnapshot::update(int value, int tid) {

  // Before update, perform scan operation.
  // In SimpleSnapshot, scan returns atomic snapshot.
  int *copy = scan();

  // Update new value for [tid]thread register.
  StampedValue *oldValue = a_table[tid];
  StampedValue *newValue = new StampedValue(oldValue->stamp + 1, value);
  a_table[tid] = newValue;

  // Free unused scan result.
  delete[] copy;
}

StampedValue **SimpleSnapshot::collect() {
  StampedValue **copy = new StampedValue*[capacity];

  // Collect StampedValue from register array.
  for (int i = 0; i < capacity; ++i) {
    copy[i] = a_table[i];
  }
  return copy;
}

int *SimpleSnapshot::scan() {
  StampedValue **oldCopy, **newCopy;

  // collect once
  oldCopy = collect();
  while (true) {
    // collect once more
    newCopy = collect();
    bool is_equal = true;
    // Check if oldCopy and newCopy are the same. (No update between two collect)
    for (int i = 0; is_equal && i < capacity; ++i) {
      is_equal &= StampedValue::equal(*oldCopy[i], *newCopy[i]);
    }
    
    // If not equal, try collect again.
    if (!is_equal) {
      delete[] oldCopy;
      oldCopy = newCopy;
      continue;
    }

    // If equal, return integer snapshot.
    int *result = new int[capacity];
    for (int i = 0; i < capacity; ++i) {
      result[i] = newCopy[i]->value;
    }

    // Free unused array.
    delete[] newCopy;
    delete[] oldCopy;
    return result;
  }
}
