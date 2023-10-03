// Calculate Fibonacci Numbers
// Public Domain
// https://creativecommons.org/publicdomain/zero/1.0/

// Compile with: gcc fibo.c -o fibo.bin -lgmp -pthread

#include <stdio.h> // printf
#include <stdlib.h> // strtol (string to long)
#include <gmp.h> // arbitrary precision library
#include <pthread.h> // multithreading
#include <unistd.h> // sysconf
#include <sys/time.h> // gettimeofday

void *fibo(void *limit)
{

  struct timeval t1, t2;
  double elapsedTime;

  // start timer
  gettimeofday(&t1, NULL);

  // setup GMP
  mpz_t a, b, c; // declare big integers

  // initialize and set, ui = unsigned int
  mpz_init_set_ui(a, 1); // initialize a to 1
  mpz_init_set_ui(b, 0); // initialize b to 0
  mpz_init(c);

  for (long i = 0; i < *(long *)limit; i++)
  {
    mpz_add(c, a, b);
    mpz_set(a, b);
    mpz_set(b, c);
  }

  
  // calculate time taken
  gettimeofday(&t2, NULL);
  elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;    // sec to ms
  elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms

  // print the results to stdout
  // it's going to be a mess, though
  printf("Fibonacci Number %ld: ",  *(long *)limit);
  mpz_out_str(stdout, 10, b);
  printf("\n");
  printf("%f ms.\n", elapsedTime);

  // cleanup
  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(c);
}

int main(int argc, char *argv[])
{
  // Get User Input
  if (argc != 2)
  {
    printf("Improper input. Exiting.\n");
    return -1;
  }

  long limit = strtol(argv[1], NULL, 10);

  // check 'man sysconf' for more info
  long nprocs = sysconf(_SC_NPROCESSORS_ONLN) / 4;
  printf("Starting fibo with %ld threads\n", nprocs);


  pthread_t threads[nprocs];

  // start threads
  for (long i = 0; i < nprocs; i++)
  {
    pthread_create(&threads[i], NULL, &fibo, (void *)&limit);
  }

  // wait for threads to finish
  for (int i = 0; i < nprocs; i++)
  {
    // the second argument is the return value of the thread
    pthread_join(threads[i], NULL);
  }

  return 0;
}
