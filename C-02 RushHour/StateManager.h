#pragma once

#include "State.h"
#include <unordered_set>
#include <omp.h>

/*
 * Describes the playing field
 * Constructor sets up the dimensions of the field along with its goal state
 */
class Playfield{

	public:
		enum GoalType { Left = 0, Top = 1, Right = 2, Bot = 3 };

		Playfield(int x, int y, GoalType g)
			: sizeX(x), sizeY(y), goal( (int)g )
		{}

		int sizeX;
		int sizeY;

		int goal; //0 left, 1 top, 2 right, 3 bottom
};

/*
	Task 3
	------
	StateManager is responsible for state that is shared across all tasks, yet
    lacks any precautions against data races.

	Add a Mutex (omp_lock_t from <omp.h> or std::Mutex from <mutex>) and use it
    where necessary to make the program thread safe.

	Bonus 
	------------------------
	C++17 offers a reader/writer lock called std::shared_mutex (in
    <shared_mutex>), which allows multiple concurrent reads	if locked with the
    proper function. Attempt to improve the efficiency of StateManager by
    locking as a reader whenever possible and only locking as a writer when a
    write is necessary.
*/

class StateManager {

	private:
		int best_solution_size;
		State best_solution;

		std::unordered_set<State,StateHash,StateEqual> state_set;

	public:
		StateManager(Playfield p)
			: playfield(p), best_solution_size(10000), best_solution() //,//
			// max_claim(0)
		{}

		// Size of the best solution found so far
		int bestSolutionSize() const {
		    return best_solution_size;
		};

		// Enter a new solution, replacing the previous one if faster to reach.
		void enterSolution(const State& res);

		/*
			Attempts to claim a configuration.
			Should claim a configuration only if it does not already exist 
			or exists but is longer that the one that is being inserted.

			Returns true if claimed, false if already taken
		*/
		bool claim(const State& val);

		void printBestSolution();

		Playfield playfield;

};
