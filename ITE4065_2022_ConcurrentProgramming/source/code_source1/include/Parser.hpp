#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include "Relation.hpp"
//---------------------------------------------------------------------------
struct SelectInfo {
   /// Relation id
   RelationId relId;
   /// Binding for the relation
   unsigned binding;
   /// Column id
   unsigned colId;
   /// The constructor
   SelectInfo(RelationId relId,unsigned b,unsigned colId) : relId(relId), binding(b), colId(colId) {};
   /// The constructor if relation id does not matter
   SelectInfo(unsigned b,unsigned colId) : SelectInfo(-1,b,colId) {};

   /// Equality operator
   inline bool operator==(const SelectInfo& o) const {
     return o.relId == relId && o.binding == binding && o.colId == colId;
   }
   /// Less Operator
   inline bool operator<(const SelectInfo& o) const {
     return binding<o.binding||colId<o.colId;
   }

   /// Dump text format
   std::string dumpText();
   /// Dump SQL
   std::string dumpSQL(bool addSUM=false);

   /// The delimiter used in our text format
   static const char delimiter=' ';
   /// The delimiter used in SQL
   constexpr static const char delimiterSQL[]=", ";
};
//---------------------------------------------------------------------------
struct FilterInfo {
   enum Comparison : char { Less='<', Greater='>', Equal='=' };
   /// Filter Column
   SelectInfo filterColumn;
   /// Constant
   uint64_t constant;
   /// Comparison type
   Comparison comparison;
   /// Dump SQL
   std::string dumpSQL();

   /// The constructor
   FilterInfo(SelectInfo filterColumn,uint64_t constant,Comparison comparison) : filterColumn(filterColumn), constant(constant), comparison(comparison) {};
   /// Dump text format
   std::string dumpText();

   /// The delimiter used in our text format
   static const char delimiter='&';
   /// The delimiter used in SQL
   constexpr static const char delimiterSQL[]=" and ";
};
static const std::vector<FilterInfo::Comparison> comparisonTypes { FilterInfo::Comparison::Less, FilterInfo::Comparison::Greater, FilterInfo::Comparison::Equal};
//---------------------------------------------------------------------------
struct PredicateInfo {
   /// Left
   SelectInfo left;
   /// Right
   SelectInfo right;
   /// The constructor
   PredicateInfo(SelectInfo left, SelectInfo right) : left(left), right(right){};
   /// Dump text format
   std::string dumpText();
   /// Dump SQL
   std::string dumpSQL();

   /// The delimiter used in our text format
   static const char delimiter='&';
   /// The delimiter used in SQL
   constexpr static const char delimiterSQL[]=" and ";
};
//---------------------------------------------------------------------------
class QueryInfo {
   public:
   /// The relation ids
   std::vector<RelationId> relationIds;
   /// The predicates
   std::vector<PredicateInfo> predicates;
   /// The filters
   std::vector<FilterInfo> filters;
   /// The selections
   std::vector<SelectInfo> selections;
   /// relation profiles
   std::vector<std::vector<Profile> > profiles;
   /// query calculation estimation
   uint64_t estimated_calcultation;
   /// Reset query info
   void clear();


   private:
   /// Parse a single predicate
   void parsePredicate(std::string& rawPredicate);
   /// Resolve bindings of relation ids
   void resolveRelationIds();

   public:
   /// Parse relation ids <r1> <r2> ...
   void parseRelationIds(std::string& rawRelations);
   /// Parse predicates r1.a=r2.b&r1.b=r3.c...
   void parsePredicates(std::string& rawPredicates);
   /// Parse selections r1.a r1.b r3.c...
   void parseSelections(std::string& rawSelections);
   /// check if given query is valid
   bool check_valid(std::vector<int> perm);
   /// calculate estimation of query execution
   bool CalculateQuery(std::vector<int> perm, std::vector<int> rel_size);
   /// find best query
   void FindBestQuery(std::string& rawQuery);
   /// analyze query
   void analyzeQuery(std::string& rawQuery);
   /// Parse selections [RELATIONS]|[PREDICATES]|[SELECTS]
   void parseQuery(std::string& rawQuery);
   /// Dump text format
   std::string dumpText();
   /// Dump SQL
   std::string dumpSQL();
   /// The empty constructor
   QueryInfo() {}
   /// The constructor that parses a query
   QueryInfo(std::string rawQuery);
};
//---------------------------------------------------------------------------
