#pragma once
#include <deque>

// Simple Garbage collecting mechanism
template <typename T>
class GC {
  private:
    // Queue for holding invisible objects per thread.
    std::deque<std::deque<T*> > gc;
  public:
    // limitation count.
    unsigned int limit;
    // number of threads.
    int capacity;

    GC(unsigned int _limit, int _capa) {
      capacity = _capa;
      limit = _limit;

      for (int i = 0; i < capacity; ++i) {
        gc.push_back(std::deque<T*>(0));
      }
    }

    // Desctructor for GC. Free all old values.
    ~GC() {
      limit = 0;
      for (int i = 0; i < capacity; ++i) {
        while (gc[i].size()) {
          clean(i);
        }
      }
    }

    void clean(int id) {
      // Check valid thread id
      if (id < 0 || id >= capacity) {
        return;
      }

      // It wille delete oldest value if size becomes
      // larger than limitation
      if (gc[id].size() > limit) {
        delete gc[id][0];
        gc[id].pop_front();
      }
    }

    // Push old value into GC queue.
    void push(int id, T *oldValue) {
      gc[id].push_back(oldValue);
    }
};

