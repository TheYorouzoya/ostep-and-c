#include <stdio.h>
#include <math.h>
#include <float.h>

double calculate (const char* expression);

void test (const char* input, double expected);

int main(int argc, char **argv) {
  test("", 0.0);
  return 0;
}


double calculate (const char* expression) {
  return 0.0;
}

void test (const char* input, double expected) {
  double result = calculate(input);
  if (fabs(expected - result) > 0.0000001D) {
    printf("Test failed: Expected %f got %f\n", expected, result);
  } else {
    printf("Test passed.\n");
  }
}
