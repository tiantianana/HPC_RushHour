CXXFLAGS := -Wall -g -Wextra -O3
OMP := -fopenmp

rush:
		module load toolchain/gompi && g++ $(OMP) $(CXXFLAGS) RushHour.cpp Car.cpp State.cpp StateManager.cpp -o RushHour

run:
	    sbatch jobscript.sh
