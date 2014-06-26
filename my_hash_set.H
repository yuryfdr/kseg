#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#ifdef HASH_NAMESPACE
#undef HASH_NAMESPACE
#endif

#ifndef __GNUC__

#include <hash_set>
#define HASH_NAMESPACE std

#elif __GNUC__ < 3

#include <hash_set>
#define HASH_NAMESPACE std


//#elif __GNUC__ < 4
#else
#include <ext/hash_set>
#define HASH_NAMESPACE __gnu_cxx
using namespace __gnu_cxx;

/*#else // gcc 3.x or later

#include <unordered_set>

#define hash_set unordered_set
#define HASH_NAMESPACE std
*/
#endif

#endif //MY_HASH_SET_H
