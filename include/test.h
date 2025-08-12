#pragma once
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <tokens.h>

#ifdef ALLOW_TESTS
  #define test(...) __VA_ARGS__
  #define testAssert(boolean) assert(boolean)

static inline bool cmpDefault(void* p1, void* p2) {
		return p1==p2;
}

static inline bool cmpString(void* p1, void* p2) {
		if (p1 == NULL || p2 == NULL) {
				return p1==p2;
		}
		return strcmp((char*)p2, (char*)p1) == 0;
}

#define areEqual(p1, p2) \
    _Generic((p1), \
        char*: cmpString, \
        const char*: cmpString, \
        default: cmpDefault)((void*)p1, (void*)p2)

  #define equal(p1, p2) if (!areEqual(p1, p2))
  #define nequal(p1, p2) if (areEqual(p1, p2))
  #define error(subTestTime, string, ...) printf("\t\e[0;31;1mSubtest %d \e[0;31;2mDIDN'T\e[0;31;1m pass: " string "\e[0m\n", subTestTime, ##__VA_ARGS__)
  #define success(subTestTime, string, ...) printf("\t\e[0;32;1mSubtest %d \e[0;32;2mDID\e[0;32;1m pass: " string "\e[0m\n", subTestTime, ##__VA_ARGS__)
#else
  #define test(...)
  #define testAssert(boolean)
  #define equal(p1, p2)
  #define nequal(p1, p2)
  #define error(testTime, subTestTime, ...)
  #define success(testTime, subTestTime, ...)
#endif
