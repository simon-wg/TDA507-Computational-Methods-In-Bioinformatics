/*
 * To compile this C program, placing the executable file in 'global', type:
 *
 *      gcc -o global global_alignment.c
 *
 * To run the program, type:
 *
 *      ./global
 */

#include <assert.h>
#include <stdio.h>

#define MAX_LENGTH 100
#define MATCH_SCORE 2
#define MISMATCH_SCORE -1
#define GAP_PENALTY 2
#define STOP 0
#define UP 1
#define LEFT 2
#define DIAG 4

int i, j;
int m, n;
int alignmentLength, exactMatches, maxCount, maxValue, score, tmp;
char X[MAX_LENGTH + 1] = "ATTA";
char Y[MAX_LENGTH + 1] = "ATTTTA";
int F[MAX_LENGTH + 1][MAX_LENGTH + 1];     /* score matrix */
int trace[MAX_LENGTH + 1][MAX_LENGTH + 1]; /* trace matrix */
char alignX[MAX_LENGTH * 2];               /* aligned X sequence */
char alignY[MAX_LENGTH * 2];               /* aligned Y sequence */
int alignmentCount = 0;

void printMatch(int len) {
  int i;
  int exactMatches = 0;
  alignmentCount++;
  printf("Alignment %d:\n", alignmentCount);
  for (i = len - 1; i >= 0; i--) {
    printf("%c", alignX[i]);
  }
  printf("\n");
  // Pipes between exact matches
  for (i = len - 1; i >= 0; i--) {
    if (alignX[i] == alignY[i]) {
      ++exactMatches;
      printf("|");
    } else
      printf(" ");
  }
  printf("\n");
  for (i = len - 1; i >= 0; i--) {
    printf("%c", alignY[i]);
  }
  printf("\n");
  // Calculate the percent identity by exact matches / alignment length
  printf("Percent identity: %.2f%%\n",
         100 * (double)exactMatches / (double)len);
  printf("Hamming distance: %d\n", len - exactMatches);
  for (int i = 0; i < 40; ++i)
    printf("-");
  printf("\n");
}

void traceback(int i, int j, int offset) {
  // Base case print match if we find a STOP
  assert(i <= m && i >= 0);
  assert(j <= n && j >= 0);
  if (trace[i][j] & STOP) {
    printMatch(offset);
    return;
  }
  // Pad alignX if we reach col 0
  if (i == 0) {
    while (j > 0) {
      alignX[offset] = '-';
      alignY[offset] = Y[j - 1];
      j--;
      offset++;
    }
    printMatch(offset);
    return;
  }
  // Pad alignY if we reach row 0
  if (j == 0) {
    while (i > 0) {
      alignX[offset] = X[i - 1];
      alignY[offset] = '-';
      i--;
      offset++;
    }
    printMatch(offset);
    return;
  }
  // Trace back recursively on the diagonal if DIAG flag is set
  if (trace[i][j] & DIAG) {
    alignX[offset] = X[i - 1];
    alignY[offset] = Y[j - 1];
    traceback(i - 1, j - 1, offset + 1);
  }
  // Trace back recursively up if UP flag is set
  if (trace[i][j] & UP) {
    alignX[offset] = X[i - 1];
    alignY[offset] = '-';
    traceback(i - 1, j, offset + 1);
  }
  // Trace back recursively left if LEFT flag is set
  if (trace[i][j] & LEFT) {
    alignX[offset] = '-';
    alignY[offset] = Y[j - 1];
    traceback(i, j - 1, offset + 1);
  }
}

int main() {
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
  F[0][0] = 0;
  trace[0][0] = STOP;
  for (i = 1; i <= m; i++) {
    F[i][0] = F[i - 1][0] - GAP_PENALTY;
    trace[i][0] = STOP;
  }
  for (j = 1; j <= n; j++) {
    F[0][j] = F[0][j - 1] - GAP_PENALTY;
    trace[0][j] = STOP;
  }
  /*
   * Fill matrices
   */
  for (i = 1; i <= m; i++) {
    for (j = 1; j <= n; j++) {
      if (X[i - 1] == Y[j - 1]) {
        score = F[i - 1][j - 1] + MATCH_SCORE;
      } else {
        score = F[i - 1][j - 1] + MISMATCH_SCORE;
      }
      trace[i][j] = DIAG;
      tmp = F[i - 1][j] - GAP_PENALTY;
      if (tmp > score) {
        score = tmp;
        trace[i][j] = UP;
      } else if (tmp == score) {
        trace[i][j] |= UP;
      }
      tmp = F[i][j - 1] - GAP_PENALTY;
      if (tmp > score) {
        score = tmp;
        trace[i][j] = LEFT;
      } else if (tmp == score) {
        trace[i][j] |= LEFT;
      }
      F[i][j] = score;
      // Use this opportunity to find the max value
      if (score > maxValue) {
        maxCount = 1;
        maxValue = score;
      } else if (score == maxValue) {
        ++maxCount;
      }
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
  printf("\n");
  traceback(m, n, 0);
  printf("Total number of optimal alignments: %d\n", alignmentCount);
  return (1);
}
