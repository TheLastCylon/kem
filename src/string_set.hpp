#ifndef _STRING_SET_HPP_
#define _STRING_SET_HPP_

#include <set>
#include <string>

typedef std::set<std::string>           StringSet;
typedef std::set<std::string>::iterator StringSetIter;

StringSet diffStringSet(const StringSet &ss1, const StringSet &ss2);

#endif // _STRING_SET_HPP_
