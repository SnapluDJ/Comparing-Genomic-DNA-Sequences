#ifndef __RECORD_H
#define __RECORD_H

#include <vector>
#include "StringTable.h"

class Record {
public:
  const char *key;
  std::vector<int> positions;
  int hashKey;
  
  Record(const char *s)
    : key{s}
  {}

  Record(const char *s, int hKey)
  	: key{s},hashKey{hKey}
  {}



};

#endif
