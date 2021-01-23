#!/bin/bash
#SBATCH -J "RushHour"
#SBATCH -N 1
#SBATCH -t 00:5:00
#SBATCH -A hpc-lco-plessl
module load toolchain/gompi && ./RushHour configs/main.json