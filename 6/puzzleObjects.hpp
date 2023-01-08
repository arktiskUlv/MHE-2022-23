#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <random>
#include <string>
#include "json.hpp"

class Item {
private:
public:
    [[nodiscard]] double getWeight() const {
        return weight;
    }

    [[nodiscard]] double getValue() const {
        return value;
    }

private:
    double weight;
    double value;
public:
    friend std::ostream &operator<<(std::ostream &os, const Item &item) {
        os << "weight: " << item.weight << " value: " << item.value << std::endl;
        return os;
    }

    Item(double weight, double value) : weight(weight), value(value) {}
};

class Knapsack {
private:
    std::vector<int> binary;
public:
    [[nodiscard]] const std::vector<int> &getBinary() const;

    explicit Knapsack(const std::vector<int> &binary) : binary(binary) {}

    friend std::ostream &operator<<(std::ostream &os, const Knapsack &knapsack);

    void expandBinary(int size);
};

std::ostream &operator<<(std::ostream &os, const Knapsack &knapsack) {
    os << "\tbinary: {";
    for (int i = 0; i < knapsack.binary.size() - 1; i++) {
        os << knapsack.binary[i] << ", ";
    }
    os << knapsack.binary.back() << "}" << std::endl;

    os << "\tdecimal: {";
    for (int i = 0; i < knapsack.binary.size() - 1; i++) {
        if (knapsack.binary[i] == 1) {
            os << i << ", ";
        }
    }
    if (knapsack.binary.back() == 1) {
        os << knapsack.binary.size() - 1 << "}" << std::endl;
    } else {
        os << "}" << std::endl;
    }

    return os;
}

const std::vector<int> &Knapsack::getBinary() const {
    return binary;
}

void Knapsack::expandBinary(int size) {
    while (true) {
        if (this->binary.size() < size) {
            this->binary.push_back(0);
        } else {
            break;
        }
    }

}

class Puzzle {
private:
public:
    [[nodiscard]] const std::vector<Item> &getItemSet() const;

    [[nodiscard]] int getCapacity() const;

    [[nodiscard]] const Knapsack &getKnapsack() const;

private:
    std::vector<Item> itemSet;
    Knapsack knapsack;
    int capacity;
public:
    Puzzle(const std::vector<Item> &itemSet, Knapsack knapsack, int capacity) : itemSet(itemSet),
                                                                                knapsack(std::move(knapsack)),
                                                                                capacity(capacity) {
        this->knapsack.expandBinary(this->itemSet.size());
    }

    friend std::ostream &operator<<(std::ostream &os, const Puzzle &puzzle);
};

std::ostream &operator<<(std::ostream &os, const Puzzle &puzzle) {
    os << "itemSet:\n";
    for (int i = 0; i < puzzle.itemSet.size(); i++) {
        os << "\t[" << i << "] " << puzzle.itemSet[i];
    }
    os << "knapsack: \n" << puzzle.knapsack;
    os << "capacity: " << puzzle.capacity << std::endl;
    return os;
}

const Knapsack &Puzzle::getKnapsack() const {
    return knapsack;
}

const std::vector<Item> &Puzzle::getItemSet() const {
    return itemSet;
}

int Puzzle::getCapacity() const {
    return capacity;
}