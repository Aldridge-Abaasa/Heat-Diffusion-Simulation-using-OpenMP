/*****************************************************************************
* DESCRIPTION:
* This program on Steady state for a 2D array w.
* using OPENMP.
* Compile: 
*	$gcc Steady_State.c -o Q3 -fopenmp -lm
* Run:
* 	$export OMP_NUM_THREADS=1
* 	$./Q3
* 	$export OMP_NUM_THREADS=2
* 	$./Q3
* 	$export OMP_NUM_THREADS=4
* 	$./Q3
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define ROWS 100
#define COLS 100

int main(void) {

  // Declare the arrays for w and wnew
  double w[ROWS][COLS];
  double wnew[ROWS][COLS];

  // Set the initial values of w and wnew in parallel
  #pragma omp parallel for // #pragma omp parallel for indicates that the iterations of the following loop will be executed in parallel by multiple threads.
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      if (i == 0 || i == ROWS - 1 || j == 0 || j == COLS - 1) {
        // Set the boundary values
        w[i][j] = 100.0;
        wnew[i][j] = 100.0;
      } else if (i == 1 && j > 1 && j < COLS - 1) {
        // Set the first row values
        w[i][j] = 0.0;
        wnew[i][j] = 0.0;
      } else {
        // Set the interior values
        w[i][j] = 75.0;
        wnew[i][j] = 75.0;
      }
    }
  }

  // Set the convergence threshold
  double threshold = 0.0001;

  // Loop until the maximum difference between w and wnew is less than the threshold
  double diff;
  double start_time = omp_get_wtime();
  do {
    // Set the maximum difference to zero
    diff = 0.0;
    // Update the interior values of wnew in parallel
    #pragma omp parallel for reduction(max:diff) // OpenMP directive that parallelizes a loop and performs a reduction operation on the variable diff.
    for (int i = 2; i < ROWS - 1; i++) {
      for (int j = 2; j < COLS - 1; j++) {
        wnew[i][j] = (w[i+1][j] + w[i-1][j] + w[i][j+1] + w[i][j-1]) / 4.0;
        // Calculate the difference between w and wnew and update the maximum difference
        double d = fabs(wnew[i][j] - w[i][j]);
        if (d > diff) {
          diff = d;
        }
      }
    }
    // Copy the values of wnew to w in parallel
    #pragma omp parallel for
    for (int i = 1; i < ROWS - 1; i++) {
      for (int j = 1; j < COLS - 1; j++) {
        w[i][j] = wnew[i][j];
      }
    }
  } while (diff >= threshold);
  double end_time = omp_get_wtime();

  // Print the final values of w
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      // printf("%f ", w[i][j]);
    }
    // printf("\n");
  }
  printf("Maximum Difference Between w and wnew reached at less than 0.0001\n");
  printf("Execution time: %f seconds\n", end_time - start_time);
  return 0;
}

/*
Computations for Time

Threads | Time (Sec) | Abs Speed | Relative Speed | Efficiency
--------|------------|-----------|----------------|-----------
1       | 1.5150     | 1.0000    | 1.0000         | 1.0000
2       | 1.5180     | 0.9980    | 0.4990         | 0.4990
4       | 1.5290     | 0.9908    | 0.2477         | 0.2477


*/