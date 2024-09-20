/**
 * @file: my_strtod.c
 * @author: Ratnesh Rastogi
 *
 * This file contains my own (naive and terrible) implementation of the strtod()
 * library function. Right now, it works for any given decimal or hexadecimal string
 * as well as any scientific notation string.
 *
 * The function also doesn't handle the overflow messages properly. Though the returned value
 * usually matches what's given by the strtod() function within the 15 digit precision.
 * Check the test suite output (or write your own test case and see) for actual numbers.
 *
 * I have done my best effort to position the "endptr" passed to the proper position, but
 * since the book I'm reading currently has put aside pointers for later, my implementation
 * may not have the expected behaviour for that variable.
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>


/**
 * Converts the initial portion of the string pointed to by "expression"
 * to a double representation.
 *
 * The expected form of the (initial portion of the) string is optional leading
 * white space, an optional plus ('+') or minus sign ('-') and then either
 *   - a decimal number
 *   - a hexadecimal number
 *   - an infinity
 *   - a NAN (not-a-number)
 *
 * A decimal number consists of a nonempty sequence of decimal digits possibly 
 * containing a radix character (decimal point, locale-dependent, usually '.'), 
 * optionally followed by a decimal exponent.  A decimal exponent consists of 
 * an 'E' or 'e', followed by an optional plus or minus sign, followed by a 
 * nonempty sequence of decimal digits, and indicates multiplication by a power 
 * of 10.
 *
 *  A  hexadecimal  number consists of a "0x" or "0X" followed by a nonempty 
 * sequence of hexadecimal digits possibly containing a radix character, 
 * optionally followed by a binary exponent.  A binary exponent consists of a 
 * 'P' or 'p', followed by an optional plus or minus sign, followed by a nonempty 
 * sequence of decimal digits, and indicates multiplication by a power of 2.  
 * At least one of radix character and binary exponent must be present.
 * 
 * An infinity is either "INF" or "INFINITY", disregarding case.
 * 
 * A NAN is "NAN" (disregarding case) optionally followed by a string
 *
 * NOTE that the binary exponent in the hexadecimal form only accepts DECIMAL
 * DIGITS and not hexadecimal digits (that is the description from strtod()).
 * 
 * @param[in]  expression
 * @param[out] endptr
 * @return Returns a double number representing the number that was found.
 */
double my_strtod (const char* expression, char** endptr);

/**
 * Does the actual parsing accoding to the rules described above.
 */
double parse_number (
  const char* expression, 
  char** endptr, 
  unsigned int start, 
  bool hexadecimalFlag
);

/**
 * Parses the given character and returns a value between 0-9 if the given
 * character is a digit. If the hexadecimal flag is set to true, then returns
 * 0-15 depending on whether the given character falls within the hexadecimal
 * character set 0-F;
 */
int parse_digit (char query, bool hexadecimalFlag);

bool case_insensitive_strncmp(const char* str1, const char* str2, unsigned int n);

/**
 * Runs all tests for the my_strtod() function.
 */
void run_tests();


/**
 * Calls my_strtod() with the given test expression and prints out the result
 * comparing the output given by my_strtod() and strtod().
 *
 * Ideally, you'd pass along an epsilon to test the accuracy within a given threshold
 * (i.e., number of digits of precision). For now, this just has a default value
 * which doesn't work as floating point precision only guarantees certain digits of precision
 * for a given value (which includes digits before and after the decimal point).
 */
void test(char* expression, char* test_description);


/**
 * Main entrypoint. Simply runs all the tests and exits.
 */
int main (int argc, char* argv[argc + 1]){
  
  run_tests();

  // test("0x8p-a", "Test: exponent");
  
  return EXIT_SUCCESS;
}


double my_strtod (const char* expression, char** endptr) {
  if (expression == NULL) {
    *endptr = (char*) expression;
    return 0.0;
  }
    
  unsigned int length = strlen(expression);

  // the final number to be returned
  double number = 0.0;

  // denotes whether the number is negative (i.e., given string starts with a negative
  // sign
  bool negativeFlag = false;

  // denotes whether the number is in hexadecimal format
  bool hexadecimalFlag = false;
  
  // index to keep track of where the parsing currently is
  int parserPosition = 0;

  // ignore all leading whitespace
  while (isspace(expression[parserPosition++]) && parserPosition < length) { 
    /* do nothing */ 
  }

  if (parserPosition >= length) {
    // bail if we somehow went past the given string's bounds
    
    *endptr = (char*) expression;  // BAD CAST BUT NO WAY AROUND IT
    return 0.0;
  }
  
  // reset parser to start of valid string
  parserPosition--;
  
  // check for a negative or postivie sign
  if (expression[parserPosition] == '-') {
    negativeFlag = true;
    parserPosition++;
  } else if (expression[parserPosition] == '+') {
    parserPosition++;
  }

  // check for 'NAN' or 'INFINITY'
  if ((length - parserPosition) >= 3) {
    if (case_insensitive_strncmp(expression, "nan", parserPosition)) {
      *endptr = (char*) &expression[parserPosition + 3];
      return NAN;
    }
    if (case_insensitive_strncmp(expression, "inf", parserPosition)) {
      if (case_insensitive_strncmp(expression, "infinity", parserPosition)) {
        *endptr = (char*) &expression[parserPosition + 8];
      } else {
        *endptr = (char*) &expression[parserPosition + 3];
      }
      return INFINITY;
    }
  }

  // check if the string is in hexadecimal format
  if ((length - parserPosition) >= 2 &&
      tolower(expression[parserPosition]) == '0' &&
      tolower(expression[parserPosition + 1]) == 'x') {
    hexadecimalFlag = true;
    parserPosition += 2;
  }

  number = parse_number(expression, endptr, parserPosition, hexadecimalFlag);

  if (negativeFlag)
    number = -number;
  
  return number;
}


double parse_number (const char* expression, 
                                    char** endptr, 
                                    unsigned int parserPosition,
                                    bool hexadecimalFlag) {
  
  if (parserPosition < 0) {
    return 0.0;
  }

  unsigned int length = strlen(expression);

  // variables to keep track of the whole and fractional part of the number
  double whole = 0.0, fraction = 0.0;

  // the final number to be returned
  double number = 0.0;

  // denotes whether we're processing the part after the decimal point
  bool decimalFlag = false;

  // denotes whether the number is in scientific notation
  bool scientificFlag = false;

  // denotes whether the scientific notation exponent is negative
  bool negativeExponentFlag = false;

  // denotes if any valid digits have been found so far
  bool foundDigits = false;

  // denotes it the given input would cause overflow (positive or negative)
  bool overflowFlag = false;

  // keeps track of digits after decimal point as powers of ten i.e., 3.123 will 
  // have the variable set to 3 as there are 3 digits after the decimal point
  int digitsAfterDecimalPoint = 0;
  
  // keeps track of the exponent value after the 'E'
  // i.e., 3.123e15 will set the exponent value to 15
  int scientificExponent = 0;

  // denotes the base of the exponent with which to multiply the number with
  const int RADIX = hexadecimalFlag ? 16 : 10;
  const int SCIENTIFIC_RADIX = hexadecimalFlag ? 2 : 10;

  const char RADIX_CHARACTER = hexadecimalFlag ? 'p' : 'e';
  
  // start parsing
  for (; parserPosition < length; parserPosition++) {
    // current character
    char current = expression[parserPosition];
    
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
    if (tolower(current) == RADIX_CHARACTER && !scientificFlag && foundDigits) {
      scientificFlag = true;
      // toggle the decimalFlag off so that the temp variable stores the corect
      // value
      decimalFlag = false;
      continue;
    }

    // scientific exponent can start with a negative '-' sign
    // only one such occurence is counted
    if (scientificFlag && (current == '-') && !negativeExponentFlag) {
      negativeExponentFlag = true;
      continue;
    }
    
    // convert current character to a number
    // since the digits in the binary exponent for a hexadecimal input are to
    // be treated as "decimal" digits (and not "hexadecimal" digits; in fact,
    // strtod() will stop parsing if passed a hex character in the binary
    // exponent), the passed flag to parse_digit() reflects that behaviour
    int digit = parse_digit (current, hexadecimalFlag && !scientificFlag);

    if (digit < 0) {
      // non-digit character detected  
      break;
    } else {
      foundDigits = true;
    }

    // update temporary number with the current one
    if (decimalFlag) {
      temp = (fraction * RADIX) + digit;
    } else if (scientificFlag) {
      temp = (scientificExponent * 10) + digit;
    } else {
      temp = (whole * RADIX) + digit;
    }

    // detect overflow
    if (temp < 0) {
      overflowFlag = true;
      break;
    }

    // update appropriate variable once clear
    if (decimalFlag) {
      fraction = temp;
    } else if (scientificFlag) {
      scientificExponent = temp;
    } else {
      whole = temp;
    }

    // increase number exponent count if we're processing a decimal
    if (decimalFlag && !scientificFlag)
      digitsAfterDecimalPoint++;
  }

  // exit if no digits were detected by the loop
  if (!foundDigits) {
    if (hexadecimalFlag) {
      *endptr = (char*) &expression[parserPosition - 1];
    } else {
      *endptr = (char*) expression;
    }
    return 0.0;
  } else {
    *endptr = (char*) &expression[parserPosition];
  }

  if (overflowFlag) {
    fprintf(stderr, "my_strtod: Numerical result out of range\n");
  }

  // printf("Whole: %g\nFraction: %g\nExponent: %d\n", whole, fraction, digitsAfterDecimalPoint);

  // negate exponent if a minus sign was present in the scientific exponent part
  if (negativeExponentFlag)
    scientificExponent = -scientificExponent;

  // calculate final value of the number
  number = (double) whole + (((double) fraction) * pow(RADIX, -digitsAfterDecimalPoint));

  // multiply by exponent if present in scientific notation
  if (scientificFlag)
    number = number * pow(SCIENTIFIC_RADIX, scientificExponent);

  return number;
}

int parse_digit (char query, bool hexadecimalFlag) {
  int digit = query - '0';

  // C standard guarantees contiguous digits for 0-9
  if (digit >= 0 && digit < 10) {
    return digit;
  }

  // parse hex characters
  if (hexadecimalFlag) {
    switch (tolower(query)) {
      case 'a':
        return 10;

      case 'b':
        return 11;

      case 'c':
        return 12;

      case 'd':
        return 13;
      
      case 'e':
        return 14;
      
      case 'f':
        return 15;

      default:
        break;
    }
  }

  return -1;
}

bool case_insensitive_strncmp(const char* str1, const char* str2, unsigned int str1_position) {
  if (str1 == NULL || str2 == NULL) {
    return false;
  }

  bool flag = true;

  // assume that str1 >= str2
  for (int i = 0; i < strlen(str2); i++) {
    if(tolower(str1[str1_position + i]) != str2[i]) {
      flag = false;
      break;
    }
  }

  return flag;
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
  test(expression, "Test: large positive number");

  expression = "100e1231231231849141241123128371231";
  test(expression, "Test: positive overflow");

  expression = "100e12312412412312312412412313abcdef";
  test(expression, "Test: positive overflow with invalid characters in the string");
  
  expression = "-123654987984651346479865132465798465413515496.0";
  test(expression, "Test: large negative number");

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

  // testing non-decimal inputs
  expression = "nan";
  test(expression, "Test: Passing different 'nan's");

  expression = "NaN";
  test(expression, "Test: Passing different 'NaN's");

  expression = "nanite";
  test(expression, "Test: Passing string containing 'non' as prefix");

  expression = "nan-234";
  test(expression, "Test: Passing a valid number string after 'nan'");

  expression = "inf";
  test(expression, "Test: Passing different 'inf's");

  expression = "INf";
  test(expression, "Test: Passing different 'INf's");

  expression = "infinity";
  test(expression, "Test: Passing different 'inifinity's");

  expression = "INFINITYDEEZ";
  test(expression, "Test: Passing string along with 'INFINITY'");

  // testing hexadecimal formatting
  expression = "0x12Abcd";
  test(expression, "Test: a basic hexadecimal string");

  expression = "0X123ABC.456G";
  test(expression, "Test: hexadecimal string with radix character '.'");

  expression = "-0x123EF.45p7";
  test(expression, "Test: hexadecimal string with negative sign and binary exponent");

  expression = "-0x123451ABCDEF12314DEF.1234DFp-15";
  test(expression, "Test: hexadecimal string with large numbers");

  expression = "0x0.1p-20";
  test(expression, "Test: hexadecimal string with small numbers");

  expression = "0xabcd";
  test(expression, "Test: hexadecimal string case test");
  
  expression = "0x12.34ABZZZp-200";
  test(expression, "Test: hexadecimal string with invalid characters");

  expression = "0xZZZ";
  test(expression, "Test: invalid hexadecimal string");

  expression = "-0xPPP";
  test(expression, "Test: invalid hexadecimal string with negative prefix");
  
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
  }

  err_flag = (fabs(result - expected) >= epsilon);

  printf("Expected: %.20g\nResult:   %.20g\n", expected, result);
  printf("Characters after number: \nExpected: %s\nResult: %s\n", endptr_expected, endptr_result); 
  
  if (!err_flag) {
    printf("Test passed\n");
  }
    
  printf("\n");
}
