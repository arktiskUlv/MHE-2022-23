#include <iostream>
#include <utility>
#include <vector>
#include <complex>
#include <random>
#include <fstream>
#include "json.hpp"

bool isInVector(std::vector<int> v, int x) {
    if (std::find(v.begin(), v.end(), x) != v.end()) {
        return true;
    }
    return false;
}

class Item {
public:
    friend std::ostream &operator<<(std::ostream &os, const Item &item);

public:
    Item(double value, double weight);

    [[nodiscard]] double getValue() const;

    [[nodiscard]] double getWeight() const;

private:
    double value;
    double weight;
};

Item::Item(double value, double weight) : value(value), weight(weight) {}

std::ostream &operator<<(std::ostream &os, const Item &item) {
    os << "value: " << item.value << "\tweight: " << item.weight;
    return os;
}

double Item::getValue() const {
    return value;
}

double Item::getWeight() const {
    return weight;
}

class Knapsack {
public:
    friend std::ostream &operator<<(std::ostream &os, const Knapsack &knapsack) {
        os << "{";
        for (int i = 0; i < knapsack.indexes.size() - 1; i++) {
            os << knapsack.indexes[i] << ", ";
        }
        os << knapsack.indexes.back() << "}";
        return os;
    }

private:
    std::vector<int> indexes;
public:
    explicit Knapsack(std::vector<int> indexes) : indexes(std::move(indexes)) {}

    [[nodiscard]] const std::vector<int> &getIndexes() const {
        return indexes;
    }

    void setIndexes(const std::vector<int> &indexes) {
        Knapsack::indexes = indexes;
    }
};

class Puzzle {
public:
    [[nodiscard]] const std::vector<Item> &getItemSet() const {
        return itemSet;
    }

    [[nodiscard]] const Knapsack &getKnapsack() const {
        return (Knapsack &&) knapsack.getIndexes();
    }

    [[nodiscard]] int getCapacity() const {
        return capacity;
    }

public:
    friend std::ostream &operator<<(std::ostream &os, const Puzzle &puzzle) {
        os << "Capacity: " << puzzle.capacity << std::endl;
        os << "Item set: " << std::endl;
        for (int i = 0; i < puzzle.itemSet.size(); i++) {
            os << "\t[" << i << "] " << puzzle.itemSet[i] << std::endl;
        }
        os << "Knapsack: " << puzzle.getKnapsack() << std::endl;

        return os;
    }

    Puzzle(std::vector<Item> itemSet, Knapsack knapsack, int capacity)
            : itemSet(std::move(itemSet)), knapsack(std::move(knapsack)), capacity(capacity) {}

    void setKnapsack(const Knapsack &knapsack) {
        Puzzle::knapsack = knapsack;
    }

private:
    std::vector<Item> itemSet;
    Knapsack knapsack;
    int capacity;
};


double evaluate(const Puzzle &puzzle) {
    std::vector<int> v = puzzle.getKnapsack().getIndexes();
    double value = 0;
    double weight = 0;
    for (int i: v) {
        value += puzzle.getItemSet()[i].getValue();
        weight += puzzle.getItemSet()[i].getWeight();
    }

    if (weight > puzzle.getCapacity()) {
        value = -1 * weight;
    }

    return value;
}

double evaluate(const Puzzle &puzzle, const Knapsack &solution) {
    std::vector<int> v = solution.getIndexes();
    double value = 0;
    double weight = 0;
    for (int i: v) {
        value += puzzle.getItemSet()[i].getValue();
        weight += puzzle.getItemSet()[i].getWeight();
    }

    if (weight > puzzle.getCapacity()) {
        value -= 2 * weight;
    }

    return value;
}

std::vector<Knapsack> genPowerSet(int size) {
    std::vector<Knapsack> powerSet;
    unsigned int powerSetSize = pow(2, size);

    for (int counter = 0; counter < powerSetSize; counter++) {
        std::vector<int> x = {};
        for (int j = 0; j < size; j++) {
            if (counter & (1 << j)) {
                x.push_back(j);
            }
        }
        Knapsack knapsack(x);
        powerSet.push_back(knapsack);
    }
    powerSet.erase(powerSet.begin());

    return powerSet;
}

double fullOverview(Puzzle puzzle) {
    std::vector<Knapsack> x = genPowerSet(puzzle.getItemSet().size());
    double bestScore = evaluate(puzzle, x.back());

    for (const Knapsack &k: x) {
        double score = evaluate(puzzle, k);
        std::cout << "solution: " << k << "\tscore: " << score << std::endl;
        if (score > bestScore) bestScore = score;
    }
    return bestScore;
}

std::vector<Puzzle> getNeighbours(Puzzle puzzle) {
    std::vector<Puzzle> neighbours;

    if (puzzle.getKnapsack().getIndexes().size() != 1) {
        for (int i = 0; i < puzzle.getKnapsack().getIndexes().size(); i++) {
            std::vector<int> tempIndexes = puzzle.getKnapsack().getIndexes();
            tempIndexes.erase(tempIndexes.begin() + i);
            Knapsack tempKnapsack(tempIndexes);
            Puzzle tempPuzzle = puzzle;
            tempPuzzle.setKnapsack(tempKnapsack);
            neighbours.push_back(tempPuzzle);
        }
    }

    for (int i = 0; i < puzzle.getKnapsack().getIndexes().size(); i++) {
        for (int j = 0; j < puzzle.getItemSet().size(); j++) {
            std::vector<int> tempIndexes = puzzle.getKnapsack().getIndexes();
            if (!isInVector(tempIndexes, j)) {
                tempIndexes[i] = j;
                Knapsack tempKnapsack(tempIndexes);
                Puzzle tempPuzzle = puzzle;
                tempPuzzle.setKnapsack(tempKnapsack);
                neighbours.push_back(tempPuzzle);
            }
        }
    }

    if (puzzle.getKnapsack().getIndexes().size() < puzzle.getItemSet().size()) {
        for (int j = 0; j < puzzle.getItemSet().size(); j++) {
            std::vector<int> tempIndexes = puzzle.getKnapsack().getIndexes();
            if (!isInVector(tempIndexes, j)) {
                tempIndexes.push_back(j);
                Knapsack tempKnapsack(tempIndexes);
                Puzzle tempPuzzle = puzzle;
                tempPuzzle.setKnapsack(tempKnapsack);
                neighbours.push_back(tempPuzzle);
            }
        }

    }

    return neighbours;
}

Puzzle deterministicHillClimbing(const Puzzle &puzzle) {
    Puzzle result = puzzle;
    double err = evaluate(puzzle);
    std::vector<Puzzle> n;
    Puzzle greatPuzzle = puzzle;
    while (true) {
        n = getNeighbours(greatPuzzle);
        double pre_err = err;
        for (const Puzzle &neighbour: n) {
            double temp_err = evaluate(neighbour);
            if (temp_err > err) {
                err = temp_err;
                result = neighbour;
            }
        }
        greatPuzzle = result;
        if (pre_err == err) {
            return result;
        }
        pre_err = err;
    }
}

Puzzle stochasticHillClimbing(const Puzzle &puzzle, int iterations) {
    Puzzle result = puzzle;
    double err = evaluate(puzzle);
    std::vector<Puzzle> n;

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 2);

    Puzzle greatPuzzle = puzzle;

    for (int i = 0; i < iterations; i++) {
        n = getNeighbours(greatPuzzle);
        std::uniform_int_distribution<> distr(0, n.size() - 1);
        Puzzle tempPuzzle = n[distr(gen)];
        greatPuzzle = tempPuzzle;

        int temp_err = evaluate(tempPuzzle);
        if (temp_err > err) {
            err = temp_err;
            result = tempPuzzle;
        }
    }
    return result;
}

int main(int argc, char** argv) {
//    std::string method = argv[1];
//    int iter = std::stoi(argv[2]);
//    std::string file_name = argv[3];
    std::ifstream file_input("C:\\Users\\s23435\\CLionProjects\\untitled\\data.json", std::ifstream::binary);
    nlohmann::json data = nlohmann::json::parse(file_input);

    int capacity = data["capacity"];
    Knapsack knapsack1(data["knapsack"]);

    std::vector<Item> itemSet2 = data["itemSet"];

//    Puzzle puzzle3(data["itemSet"], data["knapsack"]);

    Knapsack knapsack({2, 3});
    std::vector<Item> itemSet = {{4,  12},
                                 {2,  2},
                                 {2,  1},
                                 {1,  1},
                                 {10, 4}};

    Puzzle puzzle(itemSet, knapsack, 15);

    std::cout << puzzle << std::endl;

//    fullOverview(puzzle);


    std::vector<Puzzle> n = getNeighbours(puzzle);
    for (const Puzzle &k: n) {
        std::cout << k << evaluate(k) << std::endl;
    }

    Puzzle puzzle1 = deterministicHillClimbing(puzzle);
    std::cout << "\ndeterministic:\n" << puzzle1 << evaluate(puzzle1) << std::endl;
//
    Puzzle puzzle2 = stochasticHillClimbing(puzzle, 100);
    std::cout << "\nstochastic:\n" << puzzle2 << evaluate(puzzle2);

    return 0;
}
