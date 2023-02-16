#pragma once
#include <cstdint>
#include <string>
#include <vector>

using RelationId = unsigned;
// class for profiling
class Profile {
  public:
  // bucket size 
  static const int BUCKET_SIZE = 11;
  // Total tuple count of relation's tuple.
  int tuple_count;
  // # of column in relation
  int column_count;
  // max value in such column
  int upper_bound;
  // min value in such column
  int lower_bound;
  // width of each bucket.
  int bucket_width;
  // bucket array.
  std::vector<int> bucket;

  Profile(int up, int lo, int tup_count, int col_count) 
      : upper_bound(up), lower_bound(lo), tuple_count(tup_count), column_count(col_count) {
      // bucket_width is range length divided by bucket length
      bucket_width = (up - lo) / (BUCKET_SIZE - 1);
      // initialize bucket.
      for (int i = 0; i < this->BUCKET_SIZE; ++i) {
          bucket.push_back(0);
      }
  }

  // fill bucket with given column.
  void fill_bucket(uint64_t *column, int size) {
    for (int i = 0; i < size; ++i){
        int n = column[i] - lower_bound;
        bucket[n/bucket_width]++;
    }

  }

  // calculate bucket index by constant value.
  int get_bucket_index(int value) {
      int index = (value - lower_bound) / bucket_width;
      if (index < 0 || index > 11) {
          return -1;
      }
      else {
          return index;
      }
  }

  // get fraction of intersection
  float get_fraction(Profile p) {
      int up, lo;
      up = p.upper_bound;
      lo = p.lower_bound;
      if (up > upper_bound) {
          return (upper_bound - lo) / (up - lo);
      } else {
          return (up - lower_bound) / (upper_bound - lower_bound);
      }
  }
};
//---------------------------------------------------------------------------
class Relation {
  private:
  /// Owns memory (false if it was mmaped)
  bool ownsMemory;
  /// Loads data from a file
  void loadRelation(const char* fileName);

  public:
  /// The number of tuples
  uint64_t size;
  /// The join column containing the keys
  std::vector<uint64_t*> columns;
  /// Profile of relation.
  std::vector<Profile> profiles;

  /// Stores a relation into a file (binary)
  void storeRelation(const std::string& fileName);
  /// Stores a relation into a file (csv)
  void storeRelationCSV(const std::string& fileName);
  /// Dump SQL: Create and load table (PostgreSQL)
  void dumpSQL(const std::string& fileName,unsigned relationId);

  /// Constructor without mmap
  Relation(uint64_t size,std::vector<uint64_t*>&& columns) : ownsMemory(true), size(size), columns(columns) {}
  /// Constructor using mmap
  Relation(const char* fileName);
  /// Delete copy constructor
  Relation(const Relation& other)=delete;
  /// Move constructor
  Relation(Relation&& other)=default;
  /// The destructor
  ~Relation();
};
//---------------------------------------------------------------------------
