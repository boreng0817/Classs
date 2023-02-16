#include <cassert>
#include <iostream>
#include <utility>
#include <sstream>
#include "Parser.hpp"
#include <algorithm>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
static void splitString(string& line,vector<unsigned>& result,const char delimiter)
  // Split a line into numbers
{
  stringstream ss(line);
  string token;
  while (getline(ss,token,delimiter)) {
    result.push_back(stoul(token));
  }
}
//---------------------------------------------------------------------------
static void splitString(string& line,vector<string>& result,const char delimiter)
  // Parse a line into strings
{
  stringstream ss(line);
  string token;
  while (getline(ss,token,delimiter)) {
    result.push_back(token);
  }
}
//---------------------------------------------------------------------------
static void splitPredicates(string& line,vector<string>& result)
  // Split a line into predicate strings
{
  // Determine predicate type
  for (auto cT : comparisonTypes) {
    if (line.find(cT)!=string::npos) {
      splitString(line,result,cT);
      break;
    }
  }
}
//---------------------------------------------------------------------------
void QueryInfo::parseRelationIds(string& rawRelations)
  // Parse a string of relation ids
{
  splitString(rawRelations,relationIds,' ');
}
//---------------------------------------------------------------------------
static SelectInfo parseRelColPair(string& raw)
{
  vector<unsigned> ids;
  splitString(raw,ids,'.');
  return SelectInfo(0,ids[0],ids[1]);
}
//---------------------------------------------------------------------------
inline static bool isConstant(string& raw) { return raw.find('.')==string::npos; }
//---------------------------------------------------------------------------
void QueryInfo::parsePredicate(string& rawPredicate)
  // Parse a single predicate: join "r1Id.col1Id=r2Id.col2Id" or "r1Id.col1Id=constant" filter
{
  vector<string> relCols;
  splitPredicates(rawPredicate,relCols);
  assert(relCols.size()==2);
  assert(!isConstant(relCols[0])&&"left side of a predicate is always a SelectInfo");
  auto leftSelect=parseRelColPair(relCols[0]);
  if (isConstant(relCols[1])) {
    uint64_t constant=stoul(relCols[1]);
    char compType=rawPredicate[relCols[0].size()];
    filters.emplace_back(leftSelect,constant,FilterInfo::Comparison(compType));
  } else {
    predicates.emplace_back(leftSelect,parseRelColPair(relCols[1]));
  }
}
//---------------------------------------------------------------------------
void QueryInfo::parsePredicates(string& text)
  // Parse predicates
{
  vector<string> predicateStrings;
  splitString(text,predicateStrings,'&');
  for (auto& rawPredicate : predicateStrings) {
    parsePredicate(rawPredicate);
  }
}
//---------------------------------------------------------------------------
void QueryInfo::parseSelections(string& rawSelections)
 // Parse selections
{
  vector<string> selectionStrings;
  splitString(rawSelections,selectionStrings,' ');
  for (auto& rawSelect : selectionStrings) {
    selections.emplace_back(parseRelColPair(rawSelect));
  }
}
//---------------------------------------------------------------------------
static void resolveIds(vector<unsigned>& relationIds,SelectInfo& selectInfo)
  // Resolve relation id
{
  selectInfo.relId=relationIds[selectInfo.binding];
}
//---------------------------------------------------------------------------
void QueryInfo::resolveRelationIds()
  // Resolve relation ids
{
  // Selections
  for (auto& sInfo : selections) {
    resolveIds(relationIds,sInfo);
  }
  // Predicates
  for (auto& pInfo : predicates) {
    resolveIds(relationIds,pInfo.left);
    resolveIds(relationIds,pInfo.right);
  }
  // Filters
  for (auto& fInfo : filters) {
    resolveIds(relationIds,fInfo.filterColumn);
  }
}
//---------------------------------------------------------------------------
bool QueryInfo::check_valid(vector<int> perm) {
    // check if this query doesn't follow left deep tree.
    vector<int> binding_set(perm.size(), 0);

    // set used binding
    binding_set[predicates[perm[0]].left.binding] = 1;
    binding_set[predicates[perm[0]].right.binding] = 1;

    // iterate over predicates
    for (int i = 1; i < perm.size(); ++i) {
        int left = predicates[perm[i]].left.binding;
        int right = predicates[perm[i]].right.binding;

        // if both relations never used, it's invalid. return false
        if (binding_set[left] == 0 && binding_set[right] == 0)
            return false;
        binding_set[left] = 1;
        binding_set[right] = 1;
    }

    // if iteration is successfully finished, return true.
    return true;
}
//---------------------------------------------------------------------------
bool QueryInfo::CalculateQuery(vector<int> perm, vector<int> relation_size) {
    uint64_t calc = 0, current_table_size;
    vector<int> binding_set(perm.size(), 0);
    float fraction;

    SelectInfo l = predicates[perm[0]].left;
    SelectInfo r = predicates[perm[0]].right;

    binding_set[l.binding] = 1;
    binding_set[r.binding] = 1;

    // calculate given join query. 
    fraction = profiles[l.relId][l.colId].get_fraction(profiles[r.relId][l.colId]);
    current_table_size = relation_size[l.binding] * fraction;
    calc += current_table_size + relation_size[l.binding] + relation_size[r.binding];

    for (int i = 1; i < perm.size(); ++i) {
        l = predicates[perm[i]].left;
        r = predicates[perm[i]].right;

        fraction = profiles[l.relId][l.colId].get_fraction(profiles[r.relId][l.colId]);
        uint64_t temp = current_table_size * fraction;

        if (binding_set[l.binding] == 1) {
            calc += current_table_size + relation_size[r.binding];
            binding_set[r.binding] = 1;
        } else {
            calc += current_table_size + relation_size[l.binding];
            binding_set[l.binding] = 1;
        }
        current_table_size = temp;

        calc += current_table_size + relation_size[l.binding] + relation_size[r.binding];
    }

    // if cost can be updated, do update.
    if (calc < estimated_calcultation) {
        estimated_calcultation = calc;
        return true;
    }

    // else it returns false
    return false;
}
//---------------------------------------------------------------------------
void QueryInfo::FindBestQuery(string& rawQuery)
{
    // first, use QueryInfo::parseQuery to parse given rawQuery.
    this->parseQuery(rawQuery);

    // if there's one join, just return.
    if (predicates.size() == 1) {
        return;
    }

    // set estimated calculation as max value
    this->estimated_calcultation = 1000000000000;

    // filter for filter predicates
    vector<int> relation_size(relationIds.size(), 0);

    // fill relation_size from profile.
    for (int i = 0; i < relationIds.size(); ++i) {
        relation_size[i] = profiles[relationIds[i]][0].tuple_count;
    }

    // if there's a filter, apply fillter and re-calculate
    // such relation's tuple count.
    for (auto &f : filters) {
        int target_relation = f.filterColumn.relId;
        int target_column = f.filterColumn.colId;
        int binded_relation = f.filterColumn.binding;
        int filtered_tuple = 0;

        // Use several buckets to calculate filtered tuple count.
        int bucket_index = profiles[target_relation][target_column].get_bucket_index(f.constant);

        // when bucket_index is valid, perform greater or less filtering.
        // if comparision is equal, it affects to such relation's size to 1 (due to uniform distribution)
        if (bucket_index != -1) {
            switch (f.comparison) {
                case f.Comparison::Less: 
                    // simply sum up to constant.
                    for (int i = 0; i < bucket_index; ++i) {
                        filtered_tuple += profiles[target_relation][target_column].bucket[i];
                    }
                    break;
                case f.Comparison::Greater:
                    // simply sum from constant.
                    for (int i = bucket_index; i < Profile::BUCKET_SIZE; ++i) {
                        filtered_tuple += profiles[target_relation][target_column].bucket[i];
                    }
                    break;
                default:
                    break;
            }
        }

        // If filtered_tuple is 0, which means it's filtered result is 1 or 0,
        // set relation_size to 1. Or return to calculated filtered_tuple count.
        relation_size[binded_relation] = filtered_tuple != 0 ? filtered_tuple : 1;
    }

    int N = predicates.size();
    vector<int> perm(N, 0);
    vector<int> best;

    // fill permuation array
    for (int i = 0; i < N; ++i) {
        perm[i] = i;
    }

    // initial value
    best = perm;

    // check every permutations
    do {
        if (check_valid(perm) == false) {
            continue;
        }
        if (CalculateQuery(perm, relation_size)) {
            best = perm;
        }
    } while (next_permutation(perm.begin(), perm.end()));


    // new query to be written.
    vector<PredicateInfo> new_predicates;

    // fill new_query that is optimal.
    for (int i = 0; i < best.size(); ++i) {
        new_predicates.push_back(predicates[best[i]]);
    }

    // set new query to predicates.
    predicates = new_predicates;
}

//---------------------------------------------------------------------------
// function that re-write query based on profiled data of relations.
void QueryInfo::analyzeQuery(string& rawQuery)
{
    // first, use QueryInfo::parseQuery to parse given rawQuery.
    this->parseQuery(rawQuery);

    vector<int> relation_size(relationIds.size(), 0);
    vector<int> predicate_size(predicates.size(), 0);

    // fill relation_size from profile.
    for (int i = 0; i < relationIds.size(); ++i) {
        relation_size[i] = profiles[relationIds[i]][0].tuple_count;
    }

    // if there's a filter, apply fillter and re-calculate
    // such relation's tuple count.
    for (auto &f : filters) {
        int target_relation = f.filterColumn.relId;
        int target_column = f.filterColumn.colId;
        int binded_relation = f.filterColumn.binding;
        int filtered_tuple = 0;

        // Use several buckets to calculate filtered tuple count.
        int bucket_index = profiles[target_relation][target_column].get_bucket_index(f.constant);

        // when bucket_index is valid, perform greater or less filtering.
        // if comparision is equal, it affects to such relation's size to 1 (due to uniform distribution)
        if (bucket_index != -1) {
            switch (f.comparison) {
                case f.Comparison::Less: 
                    // simply sum up to constant.
                    for (int i = 0; i < bucket_index; ++i) {
                        filtered_tuple += profiles[target_relation][target_column].bucket[i];
                    }
                    break;
                case f.Comparison::Greater:
                    // simply sum from constant.
                    for (int i = bucket_index; i < Profile::BUCKET_SIZE; ++i) {
                        filtered_tuple += profiles[target_relation][target_column].bucket[i];
                    }
                    break;
                default:
                    break;
            }
        }

        // If filtered_tuple is 0, which means it's filtered result is 1 or 0,
        // set relation_size to 1. Or return to calculated filtered_tuple count.
        relation_size[binded_relation] = filtered_tuple != 0 ? filtered_tuple : 1;
    }

    // predicate_size is nearly smaller relation's size. (also due to uniform distribution.)
    for (int i = 0; i < predicates.size(); ++i) {
        // use relative index, which is binding. 
        int left_relation_idx = predicates[i].left.binding;
        int right_relation_idx = predicates[i].right.binding;

        int left_size = relation_size[left_relation_idx];
        int right_size = relation_size[right_relation_idx];

        // pick smaller size as a result of join table.
        predicate_size[i] = left_size < right_size ? left_size : right_size;
    }

    // simple sort algorithm based on predicate size.
    vector<int> ranking(predicates.size(), 0);
    vector<int> new_index(predicate_size.size(), 0);


    // calculate ranking for each predicates. Tie break by index.
    for (int i = 0; i < predicate_size.size(); ++i) for (int j = 0; j < predicate_size.size(); ++j) {
        ranking[i] += predicate_size[i] > predicate_size[j];
        // for tie-breaking
        if (predicate_size[i] == predicate_size[j]) {
            ranking[i] += i > j;
        }
    }

    // calculate index of newly written query.
    for (int i = 0; i < ranking.size(); ++i) {
        new_index[ranking[i]] = i;
    }

    // new query to be written.
    vector<PredicateInfo> new_predicates;

    // fill new_query in ascending order based on predicates size.
    for (int i = 0; i < ranking.size(); ++i) {
        new_predicates.push_back(predicates[i]);
    }

    // set new query to predicates.
    predicates = new_predicates;
}
//---------------------------------------------------------------------------
void QueryInfo::parseQuery(string& rawQuery)
  // Parse query [RELATIONS]|[PREDICATES]|[SELECTS]
{
  clear();
  vector<string> queryParts;
  splitString(rawQuery,queryParts,'|');
  assert(queryParts.size()==3);
  parseRelationIds(queryParts[0]);
  parsePredicates(queryParts[1]);
  parseSelections(queryParts[2]);
  resolveRelationIds();
}
//---------------------------------------------------------------------------
void QueryInfo::clear()
  // Reset query info
{
  relationIds.clear();
  predicates.clear();
  filters.clear();
  selections.clear();
}
//---------------------------------------------------------------------------
static string wrapRelationName(uint64_t id)
  // Wraps relation id into quotes to be a SQL compliant string
{
  return "\""+to_string(id)+"\"";
}
//---------------------------------------------------------------------------
string SelectInfo::dumpSQL(bool addSUM)
  // Appends a selection info to the stream
{
  auto innerPart=wrapRelationName(binding)+".c"+to_string(colId);
  return addSUM?"SUM("+innerPart+")":innerPart;
}
//---------------------------------------------------------------------------
string SelectInfo::dumpText()
  // Dump text format
{
  return to_string(binding)+"."+to_string(colId);
}
//---------------------------------------------------------------------------
string FilterInfo::dumpText()
  // Dump text format
{
  return filterColumn.dumpText()+static_cast<char>(comparison)+to_string(constant);
}
//---------------------------------------------------------------------------
string FilterInfo::dumpSQL()
  // Dump text format
{
  return filterColumn.dumpSQL()+static_cast<char>(comparison)+to_string(constant);
}
//---------------------------------------------------------------------------
string PredicateInfo::dumpText()
  // Dump text format
{
  return left.dumpText()+'='+right.dumpText();
}
//---------------------------------------------------------------------------
string PredicateInfo::dumpSQL()
  // Dump text format
{
  return left.dumpSQL()+'='+right.dumpSQL();
}
//---------------------------------------------------------------------------
template <typename T>
static void dumpPart(stringstream& ss,vector<T> elements)
{
  for (unsigned i=0;i<elements.size();++i) {
    ss << elements[i].dumpText();
    if (i<elements.size()-1)
      ss << T::delimiter;
  }
}
//---------------------------------------------------------------------------
template <typename T>
static void dumpPartSQL(stringstream& ss,vector<T> elements)
{
  for (unsigned i=0;i<elements.size();++i) {
    ss << elements[i].dumpSQL();
    if (i<elements.size()-1)
      ss << T::delimiterSQL;
  }
}
//---------------------------------------------------------------------------
string QueryInfo::dumpText()
  // Dump text format
{
  stringstream text;
  // Relations
  for (unsigned i=0;i<relationIds.size();++i) {
    text << relationIds[i];
    if (i<relationIds.size()-1)
      text << " ";
  }
  text << "|";

  dumpPart(text,predicates);
  if (predicates.size()&&filters.size())
    text << PredicateInfo::delimiter;
  dumpPart(text,filters);
  text << "|";
  dumpPart(text,selections);

  return text.str();
}
//---------------------------------------------------------------------------
string QueryInfo::dumpSQL()
  // Dump SQL
{
  stringstream sql;
  sql << "SELECT ";
  for (unsigned i=0;i<selections.size();++i) {
    sql << selections[i].dumpSQL(true);
    if (i<selections.size()-1)
      sql << ", ";
  }

  sql << " FROM ";
  for (unsigned i=0;i<relationIds.size();++i) {
    sql << "r" << relationIds[i] << " " << wrapRelationName(i);
    if (i<relationIds.size()-1)
      sql << ", ";
  }

  sql << " WHERE ";
  dumpPartSQL(sql,predicates);
  if (predicates.size()&&filters.size())
    sql << " and ";
  dumpPartSQL(sql,filters);

  sql << ";";

  return sql.str();
}
//---------------------------------------------------------------------------
QueryInfo::QueryInfo(string rawQuery) { parseQuery(rawQuery); }
//---------------------------------------------------------------------------
