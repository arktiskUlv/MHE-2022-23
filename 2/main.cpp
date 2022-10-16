#include <iostream>
#include <vector>

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
    std::vector<Item> items;

    explicit Knapsack(double weightCap) {
        this->weightCap = weightCap;
        this->currentWeight = 0;
        this->items = std::vector<Item>();
    }

    bool add(const Item x) {
        try {
            if (weightCap < currentWeight + x.weight) return false;
            items.push_back(x);
            this->currentWeight += x.weight;
            return true;
        } catch (...) {
            return false;
        }
    }

    void print() {
        for (Item i : items) {
            i.print();
        }
    }

};

int main() {
    using namespace std;
    Knapsack knapsack = Knapsack(4);
    knapsack.add(Item{2, 3});
    knapsack.print();

    return 0;
}
