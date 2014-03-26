/**
 * primes.c
 * This program implements the Sieve of Eratosthenes with the
 * optimization of starting at the prime square.
 * It will calculate all the primes up to 2^power, where power
 * can be either entered by the user in a command line argument
 * or the defualt POWER constant defined below.  The primes up
 * to 2^power will be writen to the file primes.txt in order.
 *
 * This program first allocates memory up to 2^(power-3), where
 * power is either the value entered by the user via the command
 * line or the default value defined in POWER if nothing is given
 * or the given value is invalid.
 *
 * This section of memory is treated like a list of numbers where
 * each bit is a number.  The program will write ones to non primes
 * and leave primes as zero, the default.
 *
 * The program then writes 1 to the first two (indicating 0 and 1),
 * and proceeds to eliminate multiples of every '0', indicating a prime,
 * after that.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#define POWER 20  /* the default value for power */
#define SPECIAL_ONE 128 /* a one followed by 7 zeroes in binary */

void findPrimes(char *start, char *endsqrt, char *end);
void setToOne(char *current, char ind);
void searchForZero(char **current, long *pointind, char *ind, char *end);
void eliminateMults(char *start, long pointind, char ind, char *end);
void printPrimes(FILE *primes, char *start, char *end);

int main(int argc, char **argv)
{
  /* declaration of variables */
  long max;
  int power;
  char *start;
  char *end;
  char *endsqrt;
  FILE *primes;

  /* processing input from the command line */
  /* if there's anything but one thing after the
   * program name, use the default value for power */
  if(argc != 2)
  {
    power = POWER;
  }
  /* if the value after the program name is not a number,
   * or a number less than three, use the default value for
   * power */
  else
  {
    power = atoi(argv[1]);
    if(power < 3)
    {
      fprintf(stderr, "Invalid value for power\n");
      power = POWER;
    }
  }

  /* display the power of 2 to be used */
  fprintf(stderr, "power = %d\n", power);

  /* calculation of the amount of memory to be allocated */
  max = 1 << (power - 3);

  /* allocation with calloc of max bytes, or 2 ^ power bits */
  start = (char *) calloc(sizeof(char), max);

  /* setting of end and endsqrt.  endsqrt is used when looking
   * for primes during the elimination stage, as no number over
   * the square root of the max needs to be considered */
  end = start + max;
  endsqrt = start + (int)sqrt(max) + 1;


  /* check for failure of calloc */
  if(start == NULL)
  {
    fprintf(stderr, "Failed to allocate memory\n");
    return 1;
  }

  /* findPrimes */
  findPrimes(start, endsqrt, end);

  /* opening of primes.txt */
  primes = fopen("./primes.txt", "w+");

  /* check for failure of fopen */
  if(primes == NULL)
  {
    fprintf(stderr, "Failed to create ./primes.txt\n");
    return 1;
  }

  /* print the list of primes to primes.txt */
  printPrimes(primes, start, end);

  /* free the list and close primes */
  free(start);
  fclose(primes);

  return 0;
}

/**
 * findPrimes
 * This is function calls all the appropriate function and constructs
 * the list of primes
 */

void findPrimes(char *start, char *endsqrt, char *end)
{
  /* defines and initializes the variables for use in the other functions */
  char *current = start;
  char ind = 0;
  long pointind = 0;

  /* set 0 and 1 as non-prime, and start the list at 2 */
  setToOne(current, ind);
  ind++;
  setToOne(current, ind);
  ind++;

  /* when searchForZero fails to find another 0 before the end of the list,
   * as indicated by endsqrt, it sets ind to -1.  This while loop runs until
   * searchForZero fails to find */
  while(ind != -1)
  {
    eliminateMults(start, pointind, ind, end);
    searchForZero(&current, &pointind, &ind, endsqrt);
  }
}

/**
 * eliminateMults
 * This function eliminates all multiples of the prime p indicated by
 * 8 * pointind + ind.  It starts at p^2, as all the earlier multiples
 * are necessarily also multiples of smaller primes.
 */

void eliminateMults(char *start, long pointind, char ind, char *end)
{
  /* definition and initialization of variables */
  char currind = ind;
  char *current = start;

  /* sets current and currind to p^2 = 8 * (8 * pointind^2 + 2 *
   * pointind * ind) + ind^2
   */
  current += (CHAR_BIT * pointind * pointind + 2 * pointind * ind);
  currind = ind * ind;

  /* if currind is larger than 8, then increase current by currind / 8
   * and reset currind to currind % 8.
   */
  if(currind >= CHAR_BIT)
  {
    current += currind / CHAR_BIT;
    currind %= CHAR_BIT;
  }

  /* Until the end of the list is reached, indicate the current number
   * as nonprime and increment by p. */
  while(current < end)
  {
    /* indicate the current number as nonprime */
    setToOne(current, currind);

    /* increment by p */
    current += pointind;
    currind += ind;

    /* if currind is out of bounds, adjust current and subtract 8 from
     * currind */
    if(currind >= CHAR_BIT)
    {
      current++;
      currind -= CHAR_BIT;
    }
  }
}

/**
 * setToOne
 * This function takes a pointer to a character and writes a one in the
 * indicated bit without changing anything else.
 * It is used to mark a number as nonprime
 */

void setToOne(char *current, char ind)
{
  /* SPECIAL_ONE is a 1 followed by seven zeroes, so temp has a one in
   * only the ind'th place, and zeroes elsewhere */
  /* write a one to the ind'th place */
  (*current) |= (SPECIAL_ONE >> ind);
}

/**
 * searchForZero
 * This function searches for primes, and sets current, pointind and ind
 * appropriate to the new prime number.  It begins it search at the number
 * immediately after the one indicated by the pair (current, ind).  It
 * searches until it finds a zero or *current is equal to end.  In the latter
 * case, it sets ind = -1 as a failure condition and exits.
 *
 * pointind is changed but never actually used.  It is assumed *current and pointind
 * agree with each other.  The current pointer is used in searchForZero, and point
 * index, pointind, is used in eliminateMults and printPrime
 */

void searchForZero(char **current, long *pointind, char *ind, char *end)
{
  char temp;

  /* skip the current number */
  (*ind) ++;
  if(*ind >= CHAR_BIT)
  {
    (*current)++;
    (*pointind)++;
    (*ind) -= CHAR_BIT;
  }

  while(*current < end)
  {
    /* this moves the bit of interest into the first bit of the byte
     * In terms of primes, this means the first bit of temp is a one
     * if the number is not a prime, and is a zero if it is. */
    temp = (**current) << (*ind);

    /* this and-equals temp with SPECIAL_ONE, or a number with seven zeroes
     * behind it.  If the ind'th bit of **current is a one, then this
     * will yield temp == SPECIAL_ONE, and if it is a zero then it will
     * yield temp == 0.
     *
     * In terms of primes, this means operation will yield 0 if the current
     * number is a prime, and will yield SPECIAL_ONE otherwise. */
    temp &= SPECIAL_ONE;

    /* if the current number is a prime, then return */
    if(temp == 0)
    {
      return;
    }

    /* increment by one */
    (*ind)++;
    if((*ind) >= CHAR_BIT)
    {
      (*current)++;
      (*pointind)++;
      (*ind) -= CHAR_BIT;
    }
  }

  /* if the end of the list is reached without finding a prime, set ind to -1 as
   * indication of failure */
  *ind = -1;
}

/**
 * printPrimes
 * This function prints the list of primes.
 * It runs searchForZero and prints 8 * pointind + ind until searchForZero hits the
 * end of the list.
 */

void printPrimes(FILE *primes, char *start, char *end)
{
  /* initialization and defintion of variables */
  char ind = 0;
  long pointind = 0;

  /* search for the first prime */
  searchForZero(&start, &pointind, &ind, end);

  /* run until searchForZero fails to find */
  while(ind != -1)
  {
    /* print it to primes and search again */
    fprintf(primes, "%ld\n", CHAR_BIT * (pointind) + ind);
    searchForZero(&start, &pointind, &ind, end);
  }
}
