#include "StateManager.h"
#include <mutex>  // For std::unique_lock

using namespace std;

bool State::legal(const StateManager *pf) const {
    //Safety shutoff
    if (steps > 500) {
        printf("Maximum steps reached. I reached: %d steps\n", steps);
        return false;
    }

    // check if all cars are on playing field and does not intersect
    for (unsigned i = 0; i < cars.size(); ++i) {
        if (cars[i].onField(pf->playfield.sizeX, pf->playfield.sizeY) == false) {
            // printf("Car %d would be off the field!\n", i);
            return false;
        }

        for (unsigned int j = 0; j < cars.size(); ++j) {
            if (i != j) {
                if (cars[i].intersects(cars[j])) {
                    // printf("Car %d would intersect car %d\n",i,j);
                    return false;
                }
            }
        }
    }

    return true;
}

State State::move_car(int car, bool forward) const {
    State result(*this);

    result.cars[car] = result.cars[car].moved(forward);

    result.steps = steps + 1;

    result.movestack.emplace_back(car, int(forward) + 10 * cars[car].getDir());

    return result;
}

bool State::won(const StateManager *pf) const {
    bool res = cars[0].checkWon(pf->playfield.goal, pf->playfield.sizeX, pf->playfield.sizeY);
    return res;
}

size_t State::hash() const {
    size_t vec_h = 0;
    int i = 0;
    for (const auto &a : cars) {
        vec_h = vec_h ^ (a.hash() << (++i));
    }
    //return std::hash<int>(playerX) ^ (std::hash<int>(playerY)<<1); // ^ std::hash<std::vector<int>>(boxes);
    return vec_h;
};

void State::printSolution() const {
    static mutex out;
    lock_guard <mutex> lg(out);

    if (movestack.size() == 0) {
        printf("No solution found!\n");
        return;
    }

    printf("---- shortest solution -----\n");

    for (unsigned i = 0; i < movestack.size(); i++) {
        string dir = "Left";

        if (movestack[i].how == 1) {
            dir = "Right";
        }
        if (movestack[i].how == 10) {
            dir = "Down";
        }
        if (movestack[i].how == 11) {
            dir = "Up";
        }

        printf("Move car %d %s \n", movestack[i].what, dir.c_str());
    }
}
