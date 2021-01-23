#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <vector>
#include <unordered_set>
#include <fstream>

#include "json.hpp"
#include "StateManager.h"
#include "Car.h"

using namespace std;
using json = nlohmann::json;

/*
	state is the arrangement of cars in the playfield that has to be checked
	manager is a pointer to the StateManager that coordinates the global state by keeping  track of results and already checked states.
*/

/*
	Task 1
	----------------------------
	Implement the body of this function. It should perform the following:

	1. Immediately return if the State to be analysed took more steps to
 reach than our current best solution.

	2. Try to claim its state in the manager. If the state is already
 claimed by another task, immediately return.

	3. Check if the state is a winning state, if so enter its solution into
 the manager and return.

	4. Iterate over all the cars (state.carCount()), for each car create
    the two follow up states:

	5. Move the respective car forward or backward using the method : state
 .move_car(...).
    The above function returns a followup state for the given car number and direction.

	6. Check whether the followup states created are legal states. If so
 recursively call Check(...) on them.
*/

void Check(State state, StateManager *manager) {

    //Step 1
    if (state.solutionSize() > manager->bestSolutionSize()) {
        return;
    }

    //Step 2
    bool claimed = manager->claim(state);
    if (!claimed) {
        return;
    }

    //Step 3
    if (state.won(manager)) {
        manager->enterSolution(state);
        return;
    }

    for (int i = 0; i < state.carCount(); i++) {

        //Moving states
        State forward_state, backward_state;
        forward_state = state.move_car(i, true);
        backward_state = state.move_car(i, false);

        //Here we are checking if the states created are legal
        //for forward
        if (forward_state.legal(manager)) {
#pragma omp task default(none) shared(manager) firstprivate(forward_state)
            Check(forward_state, manager);
        }

        //and now for backwards
        if (backward_state.legal(manager)) {
#pragma omp task default(none) shared(manager) firstprivate(backward_state)
            Check(backward_state, manager);
        }

    }

    return;
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Correct input: ./RushHour <config>\n");
        return 1;
    }

    std::ifstream configFile(argv[1]);
    json configJson = json::parse(configFile);

    // Create Playfield Object
    json pfJson = configJson["Playfield"];
    Playfield pf(pfJson["sizeX"], pfJson["sizeY"], pfJson["goal"]);

    // State_manager holds global information about the state of the solver
    StateManager *state_manager = new StateManager(pf);


    /*
      State holds information about one configuration we analyze.

      The state of the car is set by the following :
        Car(x, y, orientation / direction, length)

      where,
        - x, y coordinates of the lower left tile of the car
        - direction 0: horizontally aligned, drives left/right
        - direction 1: vertically aligned, drives down/up
        - length: the length of the car

      The origin x=0, y=0 is the position in the lower left corner of the playing field.

      The initial states of the solver are stored in json files found in the configs directory.
      This will be initialized with the following cars.
      The first car is the one that needs to reach the side of the
      playing field (set above). This is possible only if it has the proper orientation!
    */

    // Create Cars object vector
    vector <Car> cars;
    for (auto &el: configJson["Configuration"].items()) {
        auto temp = el.value();
        cars.push_back(Car(temp["x"], temp["y"], temp["dir"], temp["len"]));
    }

    /* Cars final configuration
        Solution Size : 50 or 82 when every tile of a move is counted
     https://www.michaelfogleman.com/static/rush/#ooIBBBooIKooAAJKoLCCJDDLGHEEoLGHFFoo/50
 */

    /*
      Task 0
      -------------------------------------------------------------
      Draw with pen and paper the initial configuration of cars from the sample.json configuration.
      Work out a shortest solution by hand to compare your algorithm against.

    The Sample Initial Playing Field Configuration for your convenience:
     Playfield:
     - Dimension : 8x8  (sizeX(8), sizeY(8))
     - Goal to reach : Right side of the field (goal(2))

     Configuration:
     -  Car(0,4,0,2),
     -  Car(2,4,1,3),
     -  Car(3,2,1,3),
     -  Car(0,2,0,2),
     -  Car(2,1,0,2)

    */

    /*
    Challenge
      -------------------------------------------------------------
      Parallelize the solver with OpenMP functionality.

    */

    /*
    Task 2
    -------------------------------------------------------------
    - Add OpenMP functionality to the solver by enclosing the main function's Check() call in a
    #parallel environment and a taskgroup. Spawn the Check(...) function as a task.
    Remember that the first Check(...) call only needs to be called by a single thread.

    - In Check(), spawn more tasks for recursion in a "reasonable" way.

    - Use the "default(none)" clause in the openmp parallel region to explicitly specify the
      scopes of the variables accessed by the tasks.
    */

    int thread_num = 0;
    double time_taken = 0.0;
    int rank = 0;

#pragma omp parallel
    thread_num = omp_get_num_threads();
    double start_time = omp_get_wtime();

    //TASK SYNCHRONIZATION WITH TASKWAIT --> page 13 openMP.Intermediate
#pragma omp parallel
    {
        rank = omp_get_thread_num();
#pragma omp single
        {
#pragma omp taskgroup
            {
                printf("Thread %d of %d\n", rank, thread_num);
#pragma omp task default(none) shared(state_manager, cars)
                Check(State(cars), state_manager);
#pragma omp taskwait
            }
        }
    }

    //calculating the time taken
    double end_time = omp_get_wtime();
    time_taken = end_time - start_time;

    state_manager->printBestSolution();

    printf("\nBest Solution Size of task: %d\n", state_manager->bestSolutionSize());
    printf("\nTime taken to solve task with %d threads: %lf sec \n\n", thread_num, time_taken);

    configFile.close();
    delete state_manager;

    return 0;
}