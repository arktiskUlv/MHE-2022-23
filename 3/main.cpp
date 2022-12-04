#include <iostream>
#include <vector>

bool isInVector(std::vector<int> v, int x) {
    if (std::find(v.begin(), v.end(), x) != v.end()) {
        return true;
    }
    return false;
}

struct puzzle {
    std::vector<std::vector<double>> itemSet;
    std::vector<int> knapsack;
    std::vector<int> correctKnapsack;
    double capacity;

    void print() {
        double value = 0;
        double load = 0;
        std::cout << "itemSet:" << std::endl;
        for (std::vector v: itemSet) {
            value += v[0];
            load += v[1];
            printf("%f %f\n", v[0], v[1]);
        }
        printf("\n%f %f\n", value, load);
        value = 0;
        load = 0;
        std::cout << "\nKnapsack[" << capacity << "]" << std::endl;
        for (int i: knapsack) {
            value += itemSet[i][0];
            load += itemSet[i][1];
            printf("%d. v:%f\tw:%f\n", i, itemSet[i][0], itemSet[i][1]);
        }
        printf("\nv:%f\tw:%f\n", value, load);
    }
};

int evaluate(const puzzle &puzzle) {
    int load = 0;
    for (double i: puzzle.knapsack) {
        load += puzzle.itemSet[i][1];
    }
    if (load > puzzle.capacity) return -1;

    int errors = 0;
    for (int i: puzzle.correctKnapsack) {
        if (!isInVector(puzzle.knapsack, i)) {
            errors++;
        }
    }
    return errors;
}

int evaluate(const puzzle &puzzle, std::vector<int> v) {
    int load = 0;
    for (double i: v) {
        load += puzzle.itemSet[i][1];
    }
    if (load > puzzle.capacity) return -1;

    int errors = abs(puzzle.correctKnapsack.size() - v.size());
    if (v.size() > puzzle.correctKnapsack.size()) {
        for (int i: puzzle.correctKnapsack) { // czy kazdy element poprawnego rozwiazania jest uzyty
            if (!isInVector(v, i)) {
                errors++;
            }
        }
    } else {
        for (int i: v) {
            if (!isInVector(puzzle.correctKnapsack, i)) { // czy kazdy element rozwiazania jest poprawny
                errors++;
            }
        }
    }

    return errors;
}



int main() {
    puzzle puzzle;
    puzzle.capacity = 15;
    puzzle.itemSet = {{4,  12},
                      {2,  2},
                      {2,  1},
                      {1,  1},
                      {10, 4}};

    puzzle.knapsack = {1,2,3};

    puzzle.correctKnapsack = {1,2,4, 0};

    puzzle.print();

    std::cout << evaluate(puzzle, puzzle.knapsack);
    return 0;
}
