#include <iostream>
#include <utility>
#include <vector>
#include <complex>

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

        return os;
    }

    Puzzle(std::vector<Item> itemSet, Knapsack knapsack, int capacity)
            : itemSet(std::move(itemSet)), knapsack(std::move(knapsack)), capacity(capacity) {}

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
        value = -1 * weight;
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

int main() {
    Knapsack knapsack({2, 3});

    Puzzle puzzle({{4,  12},
                   {2,  2},
                   {2,  1},
                   {1,  1},
                   {10, 4}},

                  knapsack,

                  15);

    std::cout << puzzle << std::endl;

    fullOverview(puzzle);

    return 0;
}
