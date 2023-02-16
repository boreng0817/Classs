#include "NaiveSnapshot.h"


// Constructor of NaiveSnapshot
NaiveSnapshot::NaiveSnapshot(int _capacity) {
  capacity = _capacity;
  a_table = new StampedValue*[capacity];

  for (int i = 0; i < capacity; ++i) {
    a_table[i] = new StampedValue(0);
  }

}

// Destructor of NaiveSnapshot
NaiveSnapshot::~NaiveSnapshot() {
  for (int i = 0; i < capacity; ++i) {
    delete a_table[i];
  }

  delete[] a_table;
}


void NaiveSnapshot::update(int value, int tid) {

  // Before update, perform scan operation.
  int *copy = scan();
  // Update new value for [tid]thread register.
  StampedValue *oldValue = a_table[tid];
  StampedValue *newValue = new StampedValue(oldValue->stamp + 1, value);
  a_table[tid] = newValue;

  // Free unused scan result.
  delete[] copy;
}

int *NaiveSnapshot::collect() {
  int *copy = new int[capacity];

  // Collect integer value from register array
  for (int i = 0; i < capacity; ++i) {
    copy[i] = a_table[i]->value;
  }
  return copy;
}

// In NaiveSnapshot, scan operation is the same as collect operation.
// Simply return with collect function invokation.
int *NaiveSnapshot::scan() {
  return collect();
}
