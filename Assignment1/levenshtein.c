/*
 * To compile this C program, placing the executable file in 'global', type:
 *
 *      gcc -o levenshtein levenshtein.c
 *
 * To run the program, type:
 *
 *      ./levenshtein
 */

#include <stdio.h>

#define MAX_LENGTH 100
#define MATCH_SCORE 2
#define MISMATCH_SCORE -1
#define GAP_PENALTY 2
#define STOP 0
#define UP 1
#define LEFT 2
#define DIAG 3

int main() {
  printf("Output from levenshtein.c:\n");
  int i, j;
  int m, n;
  char X[MAX_LENGTH + 1] = "ATCGAT";
  char Y[MAX_LENGTH + 1] = "ATACGT";
  int F[MAX_LENGTH + 1][MAX_LENGTH + 1]; /* score matrix */
  /*
   * Find lengths of (null-terminated) strings X and Y
   */
  m = 0;
  n = 0;
  while (X[m] != 0) {
    m++;
  }
  while (Y[n] != 0) {
    n++;
  }
  /*
   * Initialise matrices
   */
  // Note for self:
  // F[row][col]
  for (i = 0; i <= m; i++) {
    for (j = 0; j <= n; j++) {
      F[i][j] = 0;
    }
  }
  for (i = 0; i <= m; i++) {
    F[i][0] = i;
  }
  for (j = 0; j <= n; j++) {
    F[0][j] = j;
  }
  /*
   * Fill matrix
   */
  for (i = 1; i <= m; i++) {
    for (j = 1; j <= n; j++) {
      int substitutionCost;
      int minimumNearby = F[i - 1][j - 1];
      substitutionCost = !(X[j] == Y[i]);
      if (F[i - 1][j] < minimumNearby)
        minimumNearby = F[i - 1][j];
      if (F[i][j - 1] < minimumNearby)
        minimumNearby = F[i][j - 1];
      F[i][j] = minimumNearby + substitutionCost;
    }
  }
  /*
   * Print score matrix
   */
  printf("Score matrix:\n      ");
  for (j = 0; j < n; ++j) {
    printf("%5c", Y[j]);
  }
  printf("\n");
  for (i = 0; i <= m; i++) {
    if (i == 0) {
      printf(" ");
    } else {
      printf("%c", X[i - 1]);
    }
    for (j = 0; j <= n; j++) {
      printf("%5d", F[i][j]);
    }
    printf("\n");
  }
  for (int i = 0; i < 40; ++i)
    printf("-");
  printf("\n");
  printf("Levenshtein distance: %d\n\n", F[m][n]);
  return (1);
}
