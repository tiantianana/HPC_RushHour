#include "StateManager.h"
#include <mutex>
#include <algorithm>

using namespace std;
std::mutex my_mutex;

/*
    I need to use mutex (mutual exclusion) to allow control access to my share data and to avoid any type of conflict while modifying and to allow the thread do its job correctly.
    lock() --> acquires associated lock, the next thread needs to wait till lock thread has finished so it is not interrupted.
    unlock() --> release associate lock, next thread can be executed without any conflict.

    When the config is larger, if we dont use mutex it gets a segmentation fault.
 */

void StateManager::enterSolution(const State &res) {

    //wait until the lock is available. No other thread can set the lock until it is released
    my_mutex.lock();
    if (res.solutionSize() < best_solution_size) {
        printf("Solution updated, size %d \n", res.solutionSize());
        best_solution = res;
        best_solution_size = res.solutionSize();
    }

    //release the lock
    my_mutex.unlock();
}

bool StateManager::claim(const State &val) {

    /*
    returns false if the exact configuration is claimed and reached in as
    many or fewer steps, so that it won't be further worked on.
    */

    my_mutex.lock();
    auto val_it = state_set.find(val);

    if (val_it != state_set.cend() && val.solutionSize() >= val_it->solutionSize()) {
        my_mutex.unlock();
        return false; //Someone else is already doing that / has already done that
    }

    // Two val elements are treated as equal, even if the steps to reach them are different.
    // Erasing and re-inserting updates that value
    state_set.erase(val);
    state_set.insert(val);

    my_mutex.unlock();
    return true;
};

void StateManager::printBestSolution() {
    best_solution.printSolution();
}

