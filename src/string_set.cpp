#include "string_set.hpp"

StringSet diffStringSet(const StringSet &ss1, const StringSet &ss2)
{
  StringSet retval;

  for(StringSetIter itr = ss1.begin(); itr != ss1.end(); ++itr) {
    if(ss2.find(*itr) == ss2.end()) {
      retval.insert(*itr);
    }
  }

  return retval;
}
