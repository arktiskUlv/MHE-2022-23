#include <iostream>
#include <utility>
#include <vector>

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

    double getValue() const;

    double getWeight() const;

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

    const std::vector<int> &getIndexes() const {
        return indexes;
    }
};

class Puzzle {
public:
    const std::vector<Item> &getItemSet() const {
        return itemSet;
    }

    const Knapsack &getKnapsack() const {
        return (Knapsack &&) knapsack.getIndexes();
    }

    const Knapsack &getCorrectKnapsack() const {
        return correctKnapsack;
    }

    int getCapacity() const {
        return capacity;
    }

public:
    friend std::ostream &operator<<(std::ostream &os, const Puzzle &puzzle) {
        os << "Capacity: " << puzzle.capacity << std::endl;
        os << "Solution: " << puzzle.knapsack << std::endl;
        os << "Item set: " << std::endl;
        for (int i = 0; i < puzzle.itemSet.size(); i++) {
            os << "\t[" << i << "] " << puzzle.itemSet[i] << std::endl;
        }

        return os;
    }

    Puzzle(std::vector<Item> itemSet, Knapsack knapsack, Knapsack correctKnapsack, int capacity)
            : itemSet(std::move(itemSet)), knapsack(std::move(knapsack)), correctKnapsack(std::move(correctKnapsack)),
              capacity(capacity) {}

private:
    std::vector<Item> itemSet;
    Knapsack knapsack;
    Knapsack correctKnapsack;
    int capacity;
};

int evaluate(Puzzle puzzle) {
    int load = 0;

    std::vector<int> correctKnapsack = puzzle.getCorrectKnapsack().getIndexes();
    std::vector<int> knapsack = puzzle.getKnapsack().getIndexes();
    std::vector<Item> itemSet = puzzle.getItemSet();

    for (int i: correctKnapsack) {
        load += itemSet[i].getWeight();
    }

    if (load > puzzle.getCapacity()) return itemSet.size() + 1;

    int errors = abs(correctKnapsack.size() - knapsack.size());
    if (knapsack.size() > correctKnapsack.size()) {
        for (int i: correctKnapsack) {
            if (!isInVector(knapsack, i)) {
                errors++;
            }
        }
    } else {
        for (int i: knapsack) {
            if (!isInVector(correctKnapsack, i)) {
                errors++;
            }
        }
    }
    return errors;
}

int evaluate(Puzzle puzzle, Knapsack solution) {
    int load = 0;

    std::vector<int> correctKnapsack = puzzle.getCorrectKnapsack().getIndexes();
    std::vector<int> knapsack = solution.getIndexes();
    std::vector<Item> itemSet = puzzle.getItemSet();

    for (int i: correctKnapsack) {
        load += itemSet[i].getWeight();
    }

    if (load > puzzle.getCapacity()) return itemSet.size() + 1;

    int errors = abs(correctKnapsack.size() - knapsack.size());
    if (knapsack.size() > correctKnapsack.size()) {
        for (int i: correctKnapsack) {
            if (!isInVector(knapsack, i)) {
                errors++;
            }
        }
    } else {
        for (int i: knapsack) {
            if (!isInVector(correctKnapsack, i)) {
                errors++;
            }
        }
    }
    return errors;
}

int main() {
    std::vector<Item> itemSet = {{4,  12},
                                 {2,  2},
                                 {2,  1},
                                 {1,  1},
                                 {10, 4}};

    Knapsack knapsack({2, 3});

    Puzzle puzzle({{4,  12},
                   {2,  2},
                   {2,  1},
                   {1,  1},
                   {10, 4}},

                  knapsack,

                  knapsack,

                  400);

    std::cout << puzzle << std::endl;

    std::cout << evaluate(puzzle) << std::endl;
    std::cout << evaluate(puzzle, Knapsack({3, 1}));
    std::cout << puzzle.getCorrectKnapsack();
    return 0;
}
