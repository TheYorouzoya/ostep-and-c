#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

double my_strtod (const char* expression, char** endptr);

void run_tests();

void test(char* expression, char* test_description);

int main (int argc, char* argv[argc + 1]){
  
  run_tests();
  
  return EXIT_SUCCESS;
}

void run_tests () {
  
  char* expression;

  // test 1: ignore leading whitespace
  expression = "      1234";
  test(expression, "Test: ignore leading whitespace");

  // test 2: empty string/ whitespace only prints "no number found" error
  expression = "    ";
  test(expression, "Test: returns error on whitespace only string");

  expression = "";
  test(expression, "Test: returns error on empty string");
  
  // test 3: ignores any leading zeroes
  expression = "00000000000123.456";
  test(expression, "Test: ignores any leading zeroes");

  expression = "      00000000.123";
  test(expression, "Test: ignores any leading whitespace and zeroes");

  // test 4: returns a negative number if the given string starts with '-'
  expression = "-123.465";
  test(expression, "Test: returns a negative number if the given string starts with '-'");

  expression = "+123.456";
  test(expression, "Test: returns a positive number if the given string starts with '+'");
  
  expression = "      -000000123.456";
  test(expression, "Test: returns a negative number ignoring leading zeroes");

  // test 5: ignores trailing zeroes in case of decimal numbers
  expression = "123.45600000000";
  test(expression, "Test: ignores trailing zeroes in case of decimal numbers");

  // test 6: stops parsing when encountering a non-digit character
  expression = "123.456res";
  test(expression, "Test: stops parsing when encountering a non-digit character");

  // test 7: positive overflow
  expression = "12345678946551654987654654987465413.0";
  test(expression, "Test: positive overflow");

  expression = "-123654987984651346479865132465798465413515496.0";
  test(expression, "Test: negative overflow");

  expression = "0.000000000000000000000000000000000000123465";
  test(expression, "Test: underflow");

  // test 8: parse scientific notation
  expression = "123.45e3";
  test(expression, "Test: Parsing scientific notation");

  expression = "-123.45E3";
  test(expression, "Test: Parsing scientific notation with negative number");

  expression = "123.456e-5";
  test(expression, "Test: Parsing scientific notation with negative exponent");

  expression = "-0.0023e-12";
  test(expression, "Test: Parsing scientific notation with negative exponent and number");

  expression = "123.456e-10.3658";
  test(expression, "Test: Parsing scientific notation ignored decimals in the exponent");

  expression = "e5";
  test(expression, "Test: Parsing scientific notation with only exponent part");

  expression = "e-5";
  test(expression, "Test: Parsing scientific notation with only negative exponent part");

  expression = "+e5";
  test(expression, "Test: Parsing scientific notation with a '+' prefix");

  expression = "-e-5";
  test(expression, "Test: Parsing scientific notation with a '-' prefix");

  // testing edge cases
  expression = "-123.34-23";
  test(expression, "Test: Extra negatives in the expression");

  expression = "---123.456e23";
  test(expression, "Test: Extra negative signs should return error");

  expression = "123.45e-12-23";
  test(expression, "Test: Extra negative signs in the scientific exponent are ignored");
  
}

void test (char* expression, char* test_description) {
  double expected, result;
  char* endptr_expected;
  char* endptr_result;
  bool err_flag = false;
  double epsilon = 0.0000001;

  printf("%s\nInput: \"%s\"\n", test_description, expression);
  
  expected = strtod(expression, &endptr_expected);

  if (errno != 0) 
    perror("strtod");

  errno = 0;

  if (endptr_expected == expression)
    printf("strtod: No digits were found\n");
  
  
  result = my_strtod(expression, &endptr_result);

  if (endptr_result == expression) {
    printf("my_strtod: No digits were found\n\n");
    return;
  }

  err_flag = (fabs(result - expected) >= epsilon);

  printf("Expected: %.20f\nResult: %.20f\n", expected, result);
  
  if (!err_flag) {
    printf("Test passed\n");
  }
    
  printf("\n");
}

double my_strtod (const char* expression, char** endptr) {
  unsigned int length = strlen(expression);
  double whole = 0.0, fraction = 0.0;
  double number = 0.0;
  bool decimalFlag = false;
  bool negativeFlag = false;
  bool scientificFlag = false;  
  bool negativeExponentFlag = false;
  bool digitFlag = false;
  
  // index to keep track of where the parsing currently is
  int i = 0;

  // ignore all leading whitespace
  while (isspace(expression[i++]) && i < length) { /* do nothing */ }

  if (i >= length) {
    // bail if we somehow went past the given string's bounds
    
    *endptr = (char*) expression;  // BAD CAST BUT NO WAY AROUND IT
    return 0.0;
  }
  // reset i to start of string
  i--;
  
  // check for a negative or postivie sign
  if (expression[i] == '-') {
    negativeFlag = true;
    i++;
  } else if (expression[i] == '+') {
    i++;
  }

  // variable numberExponent keeps track of digits after decimal point as powers of ten
  int numberExponent = 0;
  // variable scientificExponent keeps track of the exponent after the 'E'
  int scientificExponent = 0;

  // start parsing
  for (; i < length; i++) {
    // current character
    char current = expression[i];
    
    // temporary variable to detect overflow
    double temp = 0;

    // only the first occurence of a decimal point is processed
    // also, scientific notation exponents cannot have decimals
    if (current == '.' && !decimalFlag && !scientificFlag) {
      decimalFlag = true;
      continue;
    }

    // only the first occurence of the scientific flag 'E' or 'e' is processed
    // also, only process the 'e' or 'E' if there are leading digits present
    if ((current == 'e' || current == 'E') && !scientificFlag && digitFlag) {
      scientificFlag = true;
      decimalFlag = false;
      continue;
    }

    // scientific exponent can start with a negative '-' sign
    // only one such occurence is counted
    if (scientificFlag && (current == '-') && !negativeExponentFlag) {
      negativeExponentFlag = true;
      continue;
    }
    
    // convert current character to number
    int digit = current - '0';

    // C standard guarantees digits have contiguous values
    if (digit < 0 || digit > 9) {
      // non-digit character detected  
      break;
    } else {
      digitFlag = true;
    }

    // update temporary number with the current one
    if (decimalFlag) {
      temp = (fraction * 10) + digit;
    } else if (scientificFlag) {
      temp = (scientificExponent * 10) + digit;
    } else {
      temp = (whole * 10) + digit;
    }

    // detect overflow
    if (temp < 0) {
      break;
    }

    // update parts once clear
    if (decimalFlag) {
      fraction = temp;
    } else if (scientificFlag) {
      scientificExponent = temp;
    } else {
      whole = temp;
    }

    // increase number exponent count if we're processing a decimal
    if (decimalFlag && !scientificFlag)
      numberExponent++;
  }

  // exit if no digits were detected by the loop
  if (!digitFlag) {
    *endptr = (char*) expression;
    return 0.0;
  }

  /* printf("Whole: %d\nFraction: %d\nExponent: %d\n", whole, fraction, numberExponent); */

  if (negativeExponentFlag)
    scientificExponent = -scientificExponent;
  
  number = (double) whole + (((double) fraction) * pow(10, -numberExponent));

  if (scientificFlag)
    number = number * pow(10, scientificExponent);

  if (negativeFlag)
    number = -number;
  
  return number;
}
