#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MARGIN 0.2
#define MIN_ANGLE 80.0
#define MAX_ANGLE 130.0
#define MAX_NODES 1000

typedef struct {
  double x, y, z;
} Point;

typedef struct {
  int *neighbors;
  int count;
  int capacity;
} AdjList;

typedef struct {
  int *nodes;
  int length;
} Path;

int n_points;
Point points[MAX_NODES];
AdjList adj_list[MAX_NODES];
Path longest_path;
int max_length;

void add_neighbor(AdjList *list, int neighbor) {
  if (list->count >= list->capacity) {
    list->capacity = list->capacity == 0 ? 10 : list->capacity * 2;
    list->neighbors = realloc(list->neighbors, list->capacity * sizeof(int));
  }
  list->neighbors[list->count++] = neighbor;
}

double distance(Point p1, Point p2) {
  double dx = p2.x - p1.x;
  double dy = p2.y - p1.y;
  double dz = p2.z - p1.z;
  return sqrt(dx * dx + dy * dy + dz * dz);
}

double compute_angle(Point p1, Point p2, Point p3) {
  double v1x = p1.x - p2.x;
  double v1y = p1.y - p2.y;
  double v1z = p1.z - p2.z;

  double v2x = p3.x - p2.x;
  double v2y = p3.y - p2.y;
  double v2z = p3.z - p2.z;

  double dot = v1x * v2x + v1y * v2y + v1z * v2z;
  double m1 = sqrt(v1x * v1x + v1y * v1y + v1z * v1z);
  double m2 = sqrt(v2x * v2x + v2y * v2y + v2z * v2z);

  return acos(dot / (m1 * m2)) * 180.0 / M_PI;
}

bool good_angle(int *path, int path_len) {
  if (path_len < 3)
    return true;

  double angle =
      compute_angle(points[path[path_len - 3]], points[path[path_len - 2]],
                    points[path[path_len - 1]]);

  return angle > MIN_ANGLE && angle < MAX_ANGLE;
}

void dfs(int node, bool *visited, int *path, int path_len) {
  if (!good_angle(path, path_len)) {
    return;
  }

  int remaining = n_points;
  for (int i = 0; i < n_points; i++) {
    if (visited[i])
      remaining--;
  }

  if (path_len + remaining <= max_length) {
    return;
  }

  if (path_len > max_length) {
    max_length = path_len;
    memcpy(longest_path.nodes, path, path_len * sizeof(int));
    longest_path.length = path_len;
  }

  for (int i = 0; i < adj_list[node].count; i++) {
    int neighbor = adj_list[node].neighbors[i];
    if (!visited[neighbor]) {
      visited[neighbor] = true;
      path[path_len] = neighbor;
      dfs(neighbor, visited, path, path_len + 1);
      visited[neighbor] = false;
    }
  }
}

void find_longest_path() {
  bool visited[MAX_NODES];
  int path[MAX_NODES];

  for (int start = 0; start < n_points; start++) {
    memset(visited, 0, sizeof(visited));
    visited[start] = true;
    path[0] = start;
    dfs(start, visited, path, 1);

    printf("Progress: %d/%d\r", start + 1, n_points);
    fflush(stdout);
  }
  printf("\n");
}

void build_adj_list(bool **adj_matrix) {
  for (int i = 0; i < n_points; i++) {
    for (int j = 0; j < n_points; j++) {
      if (adj_matrix[i][j]) {
        add_neighbor(&adj_list[i], j);
      }
    }
  }
}

void print_chain() {
  for (int i = 0; i < longest_path.length - 1; i++) {
    printf("%d -> ", longest_path.nodes[i] + 1);
  }
  printf("%d\n", longest_path.nodes[longest_path.length - 1] + 1);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  if (!f) {
    perror("Error opening file");
    return 1;
  }

  n_points = 0;
  int index;
  while (fscanf(f, "%d %lf %lf %lf", &index, &points[n_points].x,
                &points[n_points].y, &points[n_points].z) == 4) {
    n_points++;
  }
  fclose(f);

  printf("Read %d points\n", n_points);

  bool **adj_matrix = malloc(n_points * sizeof(bool *));
  for (int i = 0; i < n_points; i++) {
    adj_matrix[i] = calloc(n_points, sizeof(bool));
  }

  for (int i = 0; i < n_points; i++) {
    for (int j = 0; j < n_points; j++) {
      double dist = distance(points[i], points[j]);
      double err = dist - 3.8;
      if (err >= -MARGIN && err <= MARGIN) {
        adj_matrix[i][j] = true;
      }
    }
  }

  build_adj_list(adj_matrix);

  longest_path.nodes = malloc(MAX_NODES * sizeof(int));
  max_length = 0;

  find_longest_path();

  print_chain();
  printf("Length of chain is %d\n", max_length);

  for (int i = 0; i < n_points; i++) {
    free(adj_matrix[i]);
    free(adj_list[i].neighbors);
  }
  free(adj_matrix);
  free(longest_path.nodes);

  return 0;
}
