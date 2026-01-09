curl -Ls https://micro.mamba.pm/api/micromamba/linux-64/latest | tar -xvj bin/micromamba
./bin/micromamba create -y -p ./esm2 python=3.10
./bin/micromamba install -y -p ./esm2 -c conda-forge -c pytorch pytorch torchvision torchaudio cpuonly biopython pandas scipy scikit-learn matplotlib 
./esm2/bin/python -m pip install ipykernel fair-esm
./esm2/bin/python -m ipykernel install --user --name esm2 --display-name "ESM-2 micromamba"
