#include <iostream>
#include <vector>

double fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

struct Item {
    double weight;
    double value;

    bool print() {
        try {
            printf("weight: %lf, value: %lf\n", weight, value);
            return true;
        } catch (...) {
            return false;
        }
    }
};

struct Knapsack {
    double weightCap;
    double currentWeight = 0;
    double currentValue = 0;
    std::vector<Item> items = std::vector<Item>();

    explicit Knapsack(double weightCap) {
        this->weightCap = weightCap;
    }

    bool add(const Item x) {
        try {
            if (weightCap < currentWeight + x.weight) return false;
            items.push_back(x);
            this->currentWeight += x.weight;
            this->currentValue += x.value;
            return true;
        } catch (...) {
            return false;
        }
    }

    void print() {
        for (Item i: items) {
            i.print();
        }
    }
};

int main() {
    using namespace std;
    vector<Item> itemSet;

    for (int i = 0; i < 20; i++) {
        itemSet.push_back(Item{fRand(0, 100), fRand(0,100)});
    }

    Knapsack knapsack = Knapsack(4);
    knapsack.add(Item{2, 3});
    knapsack.print();

    return 0;
}
