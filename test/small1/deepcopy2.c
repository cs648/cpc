#include "testharness.h"
#include <string.h>
#include "functions/ccured_arrayofstrings.h"

//Same test as deepcopy1, using automated deep copy.

#ifndef __SEQ
 #define __SEQ
 #define __FSEQ
#endif

#define NR_BARS 4
// A test with a deep-copy
struct foo {
  int              an_int;
  double           a_double;
  char *           a_string;
  double  *        an_array;       // Length is below
  int              an_array_count; // The length of the above array
  struct bar *     bars;      // Array of length NR_BARS
};

struct bar {
  char **  argv; // Null-terminated array of strings
};

// Construct a few BARs
char * strings1[] = { "one_1", "two_1", "three_1", 0 };
char * strings2[] = { "one_2", "two_2", "three_2", 0 };
char * strings3[] = { "one_3", "two_3", "three_3", 0 };
char * strings4[] = { 0 };
struct bar arrbar[NR_BARS] = { strings1, strings2, strings3, strings4 };


// Construct something of type foo
double arr[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
struct foo x = { 5, 6.0, "a string",
                 arr, sizeof(arr) / sizeof(double),
                 arrbar };

// This function is defined in the library (a companion file deepcopy2_trusted)
extern struct foo * external(struct foo * in);
//extern struct foo * external_noarg(void);

void more_tests(void);

int main(int argc, char** argv) {
  int i;
  struct foo * res = external(&x);
  //struct foo * res = external_noarg();
  
  // Now verify the results
  if(res->an_int != x.an_int) E(1);

  if(res->a_double != x.a_double) E(2);

  if(strcmp(res->a_string, x.a_string)) E(3);

  if(res->an_array_count != x.an_array_count) E(4);
  for(i=0;i<res->an_array_count;i++) {
    if(res->an_array[i] != x.an_array[i]) E(5);
  }
  // Now compare the bar's
  for(i=0;i<NR_BARS;i++) {
    char ** pres = res->bars[i].argv;
    char ** px   = x.bars[i].argv;
    while(*px) {
      if(! *pres) E(6);
      if(strcmp(*px, *pres)) E(7);
      px ++; pres ++;
    }
    if(*pres) E(8);
  }
  more_tests();
  SUCCESS;
}


/*********** DEEP COPY WRAPPER. *******/

#pragma ccureddeepcopytofat("bar", "argv", "__arrayofstrings_fatten");
#pragma ccureddeepcopytolean("bar", "argv", "__arrayofstrings_lean");

//body generated by CCured:
struct bar* __deepcopy_to_fat_bar(struct bar* dest, struct bar __LEAN * src);
struct bar __LEAN * __deepcopy_to_lean_bar(struct bar __LEAN * dest, struct bar* src);
struct bar __LEAN * __deepcopy_array_to_lean_bar(struct bar* src);
struct bar * __deepcopy_array_to_fat_bar(struct bar __LEAN* src, int n);

struct bar* copy_bars_to_fat(struct bar __LEAN * src) {
  return __deepcopy_array_to_fat_bar(src, NR_BARS);
}

double* copy_an_array_to_fat(struct foo __LEAN * src) {
  return __mkptr_size(src->an_array, src->an_array_count 
		                     * sizeof(src->an_array[0]));
}

#pragma ccureddeepcopytofat("foo", "a_string", "__mkptr_string");
#pragma ccureddeepcopydependenttofat("foo", "an_array", "copy_an_array_to_fat");
#pragma ccureddeepcopytofat("foo", "bars", "copy_bars_to_fat");
#pragma ccureddeepcopytolean("foo", "bars", "__deepcopy_array_to_lean_bar");

//body generated by CCured:
struct foo* __deepcopy_to_fat_foo(struct foo* dest, struct foo __LEAN * src);

struct foo __LEAN * __deepcopy_to_lean_foo(struct foo __LEAN * dest, 
                                           struct foo* src);


//Redefine external with the __LEAN attribute on the return type.
// CIL will merge this into the other declaration(s).
extern struct foo __LEAN * external(struct foo __LEAN *);


#pragma ccuredwrapperof("external_wrapper", "external")
struct foo * external_wrapper(struct foo* in)
{
  struct foo __LEAN* plean = __deepcopy_to_lean_foo(NULL, in);
  plean = external(plean);
  return __deepcopy_to_fat_foo(NULL, plean);
}


void more_tests() 
{
  struct foo f;
  struct foo __LEAN flean;
  if (__deepcopy_to_fat_foo(NULL, NULL) != NULL) E(20);
  if (__deepcopy_to_fat_foo(&f, NULL) != NULL) E(21);
  if (__deepcopy_to_fat_foo(NULL, &flean) == NULL) E(22);
  __deepcopy_array_to_lean_bar(NULL); //matth: HACK to keep this decl from being deleted.
  __deepcopy_to_lean_bar(NULL, NULL); //matth: HACK to keep this decl from being deleted.
  __deepcopy_to_fat_bar(NULL, NULL); //matth: HACK to keep this decl from being deleted.
}

