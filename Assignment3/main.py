# INSTRUCTIONS TO RUN
# In order to run this program make sure matplotlib, numpy and scipy are installed on your system, or in a virtual environment.
# Run on any .pdb dataset using `python main.py <myfile.pdb> <mode>`
# The different modes available are "sse", "rmsd" and "mse"
# Ex. `python main.py 4HIR.pdb rmsd`
# "sse" is used for Q1
# "rmsd" and "mse" are used for Q3

from typing import Literal
import numpy as np
import matplotlib.pyplot as plt
import sys
from scipy.spatial.distance import pdist, squareform
from scipy.cluster.hierarchy import linkage, dendrogram


class ProteinModel:
    def __init__(self, model_number: int):
        self.model_number = model_number
        self.ca_coords = []
        self.residue_ids = []
        self.distance_matrix = None

    def add_atom(self, x: float, y: float, z: float, residue_id: str):
        self.ca_coords.append([x, y, z])
        self.residue_ids.append(residue_id)

    def compute_distance_matrix(self):
        self.distance_matrix = pdist(self.ca_coords)
        return self.distance_matrix


def parse_pdb_models(filename: str) -> list[ProteinModel]:
    models = []
    current_model = None
    with open(filename, "r") as f:
        for line in f:
            record_type = line[0:6].strip()
            if record_type == "MODEL":
                model_number = int(line[11:14].strip())
                current_model = ProteinModel(model_number)
            elif record_type == "ENDMDL":
                if current_model is None:
                    raise ValueError("ENDMDL Field outside of MODEL context")
                if len(current_model.ca_coords) > 0:
                    models.append(current_model)
                current_model = None
            elif record_type == "ATOM":
                if current_model is None:
                    raise ValueError("ATOM Field outside of MODEL context")
                atom_name = line[12:16].strip()
                if not atom_name == "CA":
                    continue
                x, y, z = map(
                    lambda a: float(a.strip()),
                    (line[30:38], line[38:46], line[46:54]),
                )
                residue_name, chain_id, residue_seq = map(
                    lambda a: a.strip(), (line[17:20], line[21:22], line[22:26])
                )
                residue_id = f"{chain_id}:{residue_name}{residue_seq}"
                current_model.add_atom(x, y, z, residue_id)
    return models


def create_score_matrix(
    models: list[ProteinModel], mode: Literal["rmsd", "sse", "mse"]
) -> np.ndarray:
    num_models = len(models)
    score_matrix = np.zeros((num_models, num_models))
    for i in range(num_models):
        for j in range(i, num_models):
            if i == j:
                score_matrix[i, j] = 0.0
            else:
                score = calculate_distance_score(models[i], models[j], mode)
                score_matrix[i, j] = score
                score_matrix[j, i] = score
    return score_matrix


def calculate_distance_score(
    model1: ProteinModel, model2: ProteinModel, mode: Literal["rmsd", "sse", "mse"]
) -> float:
    if len(model1.ca_coords) != len(model2.ca_coords):
        raise ValueError("Models must have the same number of CA atoms")
    if model1.distance_matrix is None:
        model1.compute_distance_matrix()
    if model2.distance_matrix is None:
        model2.compute_distance_matrix()
    if model1.distance_matrix is None or model2.distance_matrix is None:
        raise ValueError("How?")
    distance_squared = (model1.distance_matrix - model2.distance_matrix) ** 2
    if mode == "sse":
        return np.sum(distance_squared)
    if mode == "mse":
        return distance_squared.mean()
    return np.sqrt(distance_squared.mean())


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise ValueError("Not enough input args, run using `python main.py <file>`")
    pdb_file = sys.argv[1]
    mode: Literal["rmsd", "sse", "mse"] = "rmsd"
    metric: str = "root mean square deviation"
    method = "ward"
    if len(sys.argv) > 2:
        # I use switch statement to retain the literal type of mode..
        match sys.argv[2]:
            case "rmsd":
                mode = "rmsd"
                metric = "root mean square deviation"
                method = "average"
            case "sse":
                mode = "sse"
                metric = "sum square error"
                method = "single"
            case "mse":
                mode = "mse"
                metric = "mean square error"
    print(f"Reading PDB file: {pdb_file}")
    print("=" * 60)
    models = parse_pdb_models(pdb_file)
    print(f"\nFound {len(models)} models")
    variance_matrix = create_score_matrix(models, mode)
    print("\nScore Matrix:")
    print("-" * 80)
    np.set_printoptions(precision=4, suppress=True, linewidth=150)
    print(variance_matrix)
    print("-" * 80)
    # Find smallest non-zero distance sse
    smallest = 999999999999999
    greatest = 0
    smallest_coord = (0, 0)
    greatest_coord = (0, 0)
    for y, row in enumerate(variance_matrix):
        for x, cell in enumerate(row):
            if cell < smallest and cell != 0.0:
                smallest = cell
                smallest_coord = (x, y)
            if cell > greatest:
                greatest = cell
                greatest_coord = (x, y)
    print(
        f"Greatest {metric} found as {greatest} between model {greatest_coord[0] + 1} and {greatest_coord[1] + 1}"
    )
    print(
        f"Smallest {metric} found as {smallest} between model {smallest_coord[0] + 1} and {smallest_coord[1] + 1}"
    )
    flat_variance_matrix = squareform(variance_matrix)
    linkage_matrix = linkage(flat_variance_matrix, method=method)
    dendrogram(linkage_matrix, labels=[m.model_number for m in models])
    plt.xlabel("Model number")
    plt.ylabel("Distance")
    plt.title("Protein model dendrogram")
    plt.show()
