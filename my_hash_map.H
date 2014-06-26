#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#ifdef HASH_NAMESPACE
#undef HASH_NAMESPACE
#endif

#ifndef __GNUC__

#include <hash_map>
#define HASH_NAMESPACE std

#elif __GNUC__ < 3

#include <hash_map>
#define HASH_NAMESPACE std

#else // gcc 3.x or later

#include <ext/hash_map>
#define HASH_NAMESPACE __gnu_cxx
using namespace __gnu_cxx;

#endif

#endif //MY_HASH_MAP_H
