#pragma once

// Stamped value for Naive snapshot and Simple snapshot
class StampedValue {
  public:
    long stamp;
    int value;

    StampedValue() : stamp(0), value(0) {}
    StampedValue(int init) : stamp(0), value(init) {}
    StampedValue(long _stamp, int _value) : stamp(_stamp), value(_value) {}

    // static function that checks two StampedValues are the same
    static bool equal(StampedValue x, StampedValue y) {
      return x.stamp == y.stamp && x.value == y.value;
    }

};
