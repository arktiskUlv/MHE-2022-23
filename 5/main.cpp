#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <random>
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

std::vector<Puzzle> getNeighbours(const Puzzle &puzzle) {
    std::vector<Puzzle> neighbours;

    for (int i = 0; i < puzzle.getKnapsack().getBinary().size(); i++) {
        std::vector<int> binIndexesTemp = puzzle.getKnapsack().getBinary();
        if (binIndexesTemp[i] % 2 == 1) {
            binIndexesTemp[i] = 0;
        } else {
            binIndexesTemp[i] = 1;
        }
        Puzzle puzzleTemp(puzzle.getItemSet(), Knapsack(binIndexesTemp), puzzle.getCapacity());
        neighbours.push_back(puzzleTemp);
    }

    return neighbours;

}

std::vector<Knapsack> generateBinaryCombinations(int n) {
    std::vector<Knapsack> all_combinations;
    for (int i = 0; i < (1 << n); i++) {
        std::vector<int> binary;
        for (int j = 0; j < n; j++) {
            binary.push_back((i >> j) & 1);
        }
        all_combinations.emplace_back(binary);
    }
    return all_combinations;
}

double evaluate(const Puzzle &puzzle) {
    double value = 0;
    double weight = 0;

    for (int i = 0; i < puzzle.getKnapsack().getBinary().size(); i++) {
        if (puzzle.getKnapsack().getBinary()[i] == 1) {
            value += puzzle.getItemSet()[i].getValue();
            weight += puzzle.getItemSet()[i].getWeight();
        }
    }

    if (weight > puzzle.getCapacity()) {
        value -= 2 * weight;
    }


    return value;
}

double evaluate(const Puzzle &puzzle, const Knapsack &knapsack) {
    double value = 0;
    double weight = 0;

    for (int i = 0; i < knapsack.getBinary().size(); i++) {
        if (knapsack.getBinary()[i] == 1) {
            value += puzzle.getItemSet()[i].getValue();
            weight += puzzle.getItemSet()[i].getWeight();
        }
    }

    if (weight > puzzle.getCapacity()) {
        value -= 2 * weight;
    }


    return value;
}

Puzzle fullOverview(const Puzzle &puzzle, bool silent) {
    Puzzle bestPuzzle = puzzle;

    if (silent) {
        for (const Knapsack &knapsack: generateBinaryCombinations(puzzle.getItemSet().size())) {
            if (evaluate(puzzle, knapsack) > evaluate(bestPuzzle)) {
                bestPuzzle = Puzzle(bestPuzzle.getItemSet(), knapsack, bestPuzzle.getCapacity());
            }
        }
    } else {
        for (const Knapsack &knapsack: generateBinaryCombinations(puzzle.getItemSet().size())) {
            std::cout << Puzzle(puzzle.getItemSet(), knapsack, puzzle.getCapacity());
            if (evaluate(puzzle, knapsack) > evaluate(bestPuzzle)) {
                bestPuzzle = Puzzle(bestPuzzle.getItemSet(), knapsack, bestPuzzle.getCapacity());
            }
        }
    }


    return bestPuzzle;
}

Puzzle deterministicHillClimbing(const Puzzle &puzzle) {
    Puzzle result = puzzle;
    double score = evaluate(puzzle);
    std::vector<Puzzle> n;
    Puzzle greatPuzzle = puzzle;

    while (true) {
        n = getNeighbours(greatPuzzle);
        double preScore = score;
        for (const Puzzle &neighbour: n) {
            double tempScore = evaluate(neighbour);
            if (tempScore > score) {
                score = tempScore;
                result = neighbour;
            }
        }
        greatPuzzle = result;
        if (preScore == score) {
            return result;
        }
    }
}

bool operator==(const Puzzle &l, const Puzzle &r) {
    if (l.getKnapsack().getBinary() != r.getKnapsack().getBinary()) return false;
    return true;
}

void popFront(std::vector<Puzzle> &v)
{
    if (!v.empty()) {
        v.erase(v.begin());
    }
}

Puzzle taboo(const Puzzle& puzzle, int iterations, int maxSize, bool silent) {
    // example arguments:
    // ts C:\Users\au\CLionProjects\v3\data.json 1000 400
    // path needs to be absolute because reasons apparently
    Puzzle result = puzzle;
    Puzzle grandResult = puzzle;
    double score = evaluate(puzzle);
    double grandScore = score;
    std::vector<Puzzle> neighbours;
    Puzzle greatPuzzle = puzzle;
    std::vector<Puzzle> tabooPuzzle = {};

    while (iterations > 0) {
        neighbours = getNeighbours(greatPuzzle);

        bool c = false;
        int unvisitedNeighbours = 0;

        for (const Puzzle& neighbour: neighbours) {
            if (std::find(tabooPuzzle.begin(), tabooPuzzle.end(), neighbour) == tabooPuzzle.end()) {
                unvisitedNeighbours++;
                if (!c) {
                    c = true;
                    score = evaluate(neighbour);
                    result = neighbour;
                    continue;
                }
                double tempScore = evaluate(neighbour);
                if (tempScore > score) {
                    score = tempScore;
                    result = neighbour;
                }
            }
        }
        if (!silent) {
            std::cout << "unvisited neighbours: " << unvisitedNeighbours << " per " << neighbours.size() << " elements." << std::endl;
            if (std::find(tabooPuzzle.begin(), tabooPuzzle.end(), result) == tabooPuzzle.end()) {
                tabooPuzzle.push_back(result);
            }
            if (tabooPuzzle.size() == maxSize) {
                popFront(tabooPuzzle);
                std::cout << "reduction!" << std::endl;
            }

            greatPuzzle = result;
            if (unvisitedNeighbours == 0) {
                std::cout << "its stuck at: #" << iterations << " iteration." << std::endl;
                return grandResult;
            }
            double tempGrandScore = evaluate(result);
            if (tempGrandScore > grandScore) {
                grandResult = result;
                grandScore = tempGrandScore;
            }
            iterations--;
            std::cout << iterations << std::endl;
        } else {
            if (std::find(tabooPuzzle.begin(), tabooPuzzle.end(), result) == tabooPuzzle.end()) {
                tabooPuzzle.push_back(result);
            }
            if (tabooPuzzle.size() == maxSize) {
                popFront(tabooPuzzle);
            }

            greatPuzzle = result;
            if (unvisitedNeighbours == 0) {
                return grandResult;
            }
            double tempGrandScore = evaluate(result);
            if (tempGrandScore > grandScore) {
                grandResult = result;
                grandScore = tempGrandScore;
            }
            iterations--;
        }

    }
    return grandResult;
}

Puzzle stochasticHillClimbing(const Puzzle &puzzle, int iterations) {
    Puzzle result = puzzle;
    double score = evaluate(puzzle);
    std::vector<Puzzle> n;

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd());
    (std::uniform_int_distribution<>(0, 2));

    Puzzle greatPuzzle = puzzle;

    for (int i = 0; i < iterations; i++) {
        n = getNeighbours(greatPuzzle);
        std::uniform_int_distribution<> distribution(0, n.size() - 1);
        Puzzle tempPuzzle = n[distribution(gen)];
        greatPuzzle = tempPuzzle;

        double tempScore = evaluate(tempPuzzle);
        if (tempScore > score) {
            score = tempScore;
            result = tempPuzzle;
        }
    }
    return result;
}

int main([[maybe_unused]] int argc, char **argv) {
    std::string method = argv[1];
    std::string file_name = argv[2];
    int iter = std::stoi(argv[3]);
    int maxSize = std::stoi(argv[4]);

    std::ifstream file_input(file_name, std::ifstream::binary);
    nlohmann::json data = nlohmann::json::parse(file_input);

    int capacity = data["capacity"];
    Knapsack knapsack(data["knapsack"]);
    std::vector<Item> itemSet;
    for (auto & i : data["itemSet"]) {
        Item item(i[0], i[1]);
        itemSet.push_back(item);
    }
    Puzzle puzzle(itemSet, knapsack, capacity);


    if (method == "d") {
        Puzzle deterministicPuzzle = deterministicHillClimbing(puzzle);
        std::cout << "bestPuzzle according to deterministicHillClimbing:\n" << deterministicPuzzle << std::endl;
        std::cout << "score: " << evaluate(deterministicPuzzle) << std::endl;
    }
    if (method == "s") {
        Puzzle stochasticPuzzle = stochasticHillClimbing(puzzle, iter);
        std::cout << "bestPuzzle according to stochasticHillClimbing of " << iter << " iterations:\n" << stochasticPuzzle << std::endl;
        std::cout << "score: " << evaluate(stochasticPuzzle) << std::endl;
    }
    if (method == "o") {
        Puzzle bestPuzzle = fullOverview(puzzle, false);
        std::cout << "bestPuzzle:\n" << bestPuzzle << std::endl;
        std::cout << "bestSolutionScore: " << evaluate(bestPuzzle);
    }
    if (method == "os") {
        Puzzle bestPuzzle = fullOverview(puzzle, true);
        std::cout << "bestPuzzle:\n" << bestPuzzle << std::endl;
        std::cout << "bestSolutionScore: " << evaluate(bestPuzzle);
    }
    if (method == "t") {
        Puzzle tabooPuzzle = taboo(puzzle, iter, maxSize, false);
        std::cout << "tabooPuzzle according to stochasticHillClimbing of " << iter << " iterations and " << maxSize << " maxSize:\n" << tabooPuzzle << std::endl;
        std::cout << "score: " << evaluate(tabooPuzzle);
    }
    if (method == "ts") {
        Puzzle tabooPuzzle = taboo(puzzle, iter, maxSize, true);
        std::cout << "tabooPuzzle according to stochasticHillClimbing of " << iter << " iterations and " << maxSize << " maxSize:\n" << tabooPuzzle << std::endl;
        std::cout << "score: " << evaluate(tabooPuzzle);
    }

    return 0;
}
