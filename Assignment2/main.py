# By Simon Westlin Green
import sys
import numpy as np

# import math
from tqdm import trange

MARGIN = 0.2
MIN_ANGLE = 83
MAX_ANGLE = 127

known_angles = {}


def main():
    try:
        file = sys.argv[1]
    except IndexError as e:
        raise ValueError(
            "No input args specified, example usage: `python main.py data_q1.txt`"
        ) from e
    # the indexes in this matrix are the ones in the files - 1 because I prefer zero indexing
    matrix = read_data_points(file)
    dist = compute_distance_matrix(matrix)
    # Sub 3.8 from each cell to get them to contain error instead of distance
    err = dist - 3.8
    # print(err)
    adj_matrix = (err >= -MARGIN) & (err <= MARGIN)
    # print_bool_matrix(adj_matrix)

    adj_list = to_adj_list(adj_matrix)
    length, chain = longest_acyclic_path(adj_list, matrix)
    # print_chain(chain)
    ugly_print_chain(chain)
    # print_angles(df, chain)
    print(f"Length of chain is {length}")


# Helper functions to display information
def print_bool_matrix(matrix):
    for row in matrix[::-1]:
        for cell in row[::-1]:
            print(f"{'T' if cell else 'F'}", end="\t")
        print()


def ugly_print_chain(chain):
    for value in chain:
        print(f"{value + 1}")


def print_chain(chain):
    for value in chain[:-1]:
        print(f"{value + 1} -> ", end="")
    print(chain[-1] + 1)


def print_angles(chain, matrix):
    def gen():
        loc_chain = chain.copy()
        while len(loc_chain) >= 3:
            p1 = matrix[loc_chain.pop()]
            p2 = matrix[loc_chain[-1]]
            p3 = matrix[loc_chain[-2]]
            yield (p1, p2, p3)

    for p1, p2, p3 in gen():
        angle = compute_angle(p1, p2, p3)
        print(np.degrees(angle))


def compute_distance_matrix(matrix):
    # Vectorized version
    # I used the help of LLMs to optimize this, because I don't care enough about numpy semantics.
    # It does in essence the exact same thing as below, with some added numpy magic
    diff = matrix[:, None, :] - matrix[None, :, :]
    return np.linalg.norm(diff, axis=2)

    # dist = np.zeros((len(matrix), len(matrix)))
    # for i in range(len(matrix)):
    #     for j in range(len(matrix)):
    #         x1 = matrix[i][0]
    #         y1 = matrix[i][1]
    #         z1 = matrix[i][2]
    #         x2 = matrix[j][0]
    #         y2 = matrix[j][1]
    #         z2 = matrix[j][2]
    #         distance = math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2 + (z2 - z1) ** 2)
    #         dist[i][j] = distance
    # return dist


def read_data_points(file):
    with open(file, "r") as f:
        lines = f.readlines()
        matrix = [[]] * len(lines)
        for line in lines:
            index, x, y, z = line.split()
            matrix[int(index) - 1] = [float(x), float(y), float(z)]
    return np.array(matrix)


def to_adj_list(adj_matrix):
    n = len(adj_matrix)
    adj_list = [[] for _ in range(n)]
    for i in range(n):
        adj_list[i] = np.where(adj_matrix[i])[0].tolist()
    return adj_list


def get_chains(adj_list):
    chains = []
    visited = [False] * len(adj_list)

    for start in range(len(adj_list)):
        search_chain(start, adj_list, visited, [], chains)

    return chains


def search_chain(current, adj_list, visited, curr_chain, chains):
    visited[current] = True
    curr_chain.append(current)
    chains.append(curr_chain.copy())

    for next in adj_list[current]:
        if not visited[next]:
            search_chain(next, adj_list, visited, curr_chain, chains)

    curr_chain.pop()
    visited[current] = False


def compute_angle(p1, p2, p3):
    v1 = p1 - p2
    v2 = p3 - p2
    m1 = np.linalg.norm(v1)
    m2 = np.linalg.norm(v2)
    return np.arccos(np.dot(v1, v2) / (m1 * m2))


def good_angle(path, matrix):
    global known_angles
    if len(path) < 3:
        return True
    key = (path[-3], path[-2], path[-1])
    angle = known_angles.get(key)
    if angle is not None:
        return angle > MIN_ANGLE and angle < MAX_ANGLE
    angle = np.degrees(
        compute_angle(matrix[path[-3]], matrix[path[-2]], matrix[path[-1]])
    )
    known_angles[key] = angle
    return angle > MIN_ANGLE and angle < MAX_ANGLE


def longest_acyclic_path(graph, matrix):
    max_length = 0
    longest_path = []

    def dfs(node, visited, path):
        nonlocal max_length, longest_path

        if not good_angle(path, matrix):
            return

        if len(path) > max_length:
            max_length = len(path)
            longest_path = path.copy()

        remaining = len(graph) - len(visited)
        if len(path) + remaining <= max_length:
            return

        for neighbor in graph[node]:
            if neighbor not in visited:
                visited.add(neighbor)
                path.append(neighbor)
                dfs(neighbor, visited, path)
                path.pop()
                visited.remove(neighbor)

    # Try starting from each vertex
    for start in trange(len(graph)):
        visited = {start}
        dfs(start, visited, [start])

    return max_length, longest_path


if __name__ == "__main__":
    main()
