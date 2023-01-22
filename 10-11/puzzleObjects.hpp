#pragma once
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
    std::string out;
    out += "\tbinary: {";
    for (int i = 0; i < knapsack.binary.size() - 1; i++) {
        out += std::to_string(knapsack.binary[i]);
        out += ", ";
    }
    out += std::to_string(knapsack.binary.back());
    out += "}\n";

    out += "\tdecimal: {";
    for (int i = 0; i < knapsack.binary.size() - 1; i++) {
        if (knapsack.binary[i] == 1) {
            out += std::to_string(i);
            out += ", ";
        }
    }
    if (knapsack.binary.back() == 1) {
        out += std::to_string(knapsack.binary.size() - 1);
        out += "}\n";
    } else {
        out.pop_back();
        out.pop_back();
        out += "}\n";
    }

    os << out;
    return os;
}

class Puzzle {
private:
public:
    void setKnapsack(const Knapsack &knapsack);

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

