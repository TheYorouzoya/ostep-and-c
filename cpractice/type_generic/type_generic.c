#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <stdbool.h>


double min(double a, double b);
long double minld(long double a, long double b);
float minf(float a, float b);
int mini(int a, int b);
unsigned int minui(unsigned int a, unsigned int b);
long minl(long a, long b);
unsigned long minul(unsigned long a, unsigned long b);
long long minll(long long a, long long b);
unsigned long long minull(unsigned long long a, unsigned long long b);


inline
double min(double a, double b) {
  return a < b ? a : b;
}

inline
long double minld(long double a, long double b) {
  return a < b ? a : b;
}

inline
float minf(float a, float b) {
  return a < b ? a : b;
}

inline
int mini(int a, int b) {
  return a < b ? a : b;
}

inline
unsigned int minui(unsigned int a, unsigned int b) {
  return a < b ? a : b;
}

inline
long minl(long a, long b) {
  return a < b ? a : b;
}

inline
unsigned long minul(unsigned long a, unsigned long b) {
  return a < b ? a : b;
}

inline
long long minll(long long a, long long b) {
  return a < b ? a : b;
}

inline
unsigned long long minull(unsigned long long a, unsigned long long b) {
  return a < b ? a : b;
}


/**
 ** @brief Type-generic minimum for different types
 **/
#define MIN(A, B)				\
  _Generic((A)+(B),				\
	   float: minf,				\
	   long double: minld,			\
	   int: mini,				\
	   unsigned int: minui,			\
	   long: minl,				\
	   unsigned long: minul,		\
	   long long: minll,			\
	   unsigned long long: minull,		\
	   default: min) ((A), (B))		



/*
  Write a macro PROMOTE(XT, A) that returns the value of A as type XT.
  For example, PROMOTE(1u, 3) would be 3u.
*/

#define PROMOTE(XT, A)				\
  _Generic(XT,					\
	   int: 0+(A)+0,			\
	   char: '\0'+(A)+'\0',			\
	   unsigned: 0u+(A)+0u,			\
	   float: 0.0+(A)+0.0)



/*
  Write a macro SIGNEDNESS(XT) that return false or true according to the
  signedness of thet ype of XT. For example, SIGNEDNESS(1l) would be true.
*/
#define SIGNEDNESS(XT)				\
  _Generic(XT,					\
	   unsigned: true,			\
	   unsigned long: true,			\
	   unsigned long long: true,		\
	   default: false)


/*
  Write a macro mix(A, B) that computes the maximum value of A and B. If both
  have the same signedness, the result type should be the wider type of the
  two. If both have different signedness, the return type should be an
  unsigned type that fits all positive values of both types.
*/


int main(int arc, char** argv) {
  long double a = 32.0;
  long double b = 32.00013;
  printf("long double: 32.0 vs 32.00013: %Lg\n", MIN(a, b));

  printf("unsigned long long: 3456ull vs 34567ull: %llu\n", MIN(3456ull, 34567ull));

  char c = '3';
  float d = 1.0;
  printf("converting 3 to char: %c\t%c\n", PROMOTE(c, 69), (char) 69);
  printf("converting: %f\n", PROMOTE(d, c));

  unsigned e = 1234;

  printf("Signedness of c:%d\td:%d\te:%d\n", SIGNEDNESS(c), SIGNEDNESS(d), SIGNEDNESS(e));
  
  return EXIT_SUCCESS;
}
