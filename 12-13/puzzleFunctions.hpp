#pragma once

#include "puzzleObjects.hpp"
#include <omp.h>

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
#pragma omp parallel for reduction(+:value, weight)
    for (int i = 0; i < puzzle.getKnapsack().getBinary().size(); i++) {
        if (puzzle.getKnapsack().getBinary()[i] == 1) {
            value += puzzle.getItemSet()[i].getValue();
            weight += puzzle.getItemSet()[i].getWeight();
        }
    }

    if (weight > puzzle.getCapacity()) {
        value = 0;
    }
    return value;
}


double evaluate(const Puzzle &puzzle, const Knapsack &knapsack) {
    double value = 0;
    double weight = 0;
#pragma omp parallel for reduction(+:value, weight)
    for (int i = 0; i < knapsack.getBinary().size(); i++) {
        if (knapsack.getBinary()[i] == 1) {
            value += puzzle.getItemSet()[i].getValue();
            weight += puzzle.getItemSet()[i].getWeight();
        }
    }

    if (weight > puzzle.getCapacity()) {
        value = 0;
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
    Puzzle currentPuzzle = puzzle;
    double currentScore = evaluate(puzzle);

    while (true) {
        std::vector<Puzzle> neighbours = getNeighbours(currentPuzzle);
        double previousScore = currentScore;
        for (const Puzzle &neighbor: neighbours) {
            double neighborScore = evaluate(neighbor);
            if (neighborScore > currentScore) {
                currentScore = neighborScore;
                currentPuzzle = neighbor;
            }
        }
        if (previousScore == currentScore) {
            return currentPuzzle;
        }
    }
}


bool operator==(const Puzzle &l, const Puzzle &r) {
    if (l.getKnapsack().getBinary() != r.getKnapsack().getBinary()) return false;
    return true;
}

void popFront(std::vector<Puzzle> &v) {
    if (!v.empty()) {
        v.erase(v.begin());
    }
}


Puzzle taboo(const Puzzle &puzzle, int iterations, int maxSize, bool silent) {
    Puzzle result = puzzle;
    Puzzle grandResult = puzzle;
    double score = evaluate(puzzle);
    double grandScore = score;
    std::vector<Puzzle> neighbours;
    Puzzle greatPuzzle = puzzle;
    std::vector<Puzzle> taboo = {};

    while (iterations > 0) {
        neighbours = getNeighbours(greatPuzzle);

        bool checked = false;
        int unvisitedNeighbours = 0;

        for (const Puzzle &neighbour: neighbours) {
            // checks whether the element neighbour is present in the container taboo
            if (std::find(taboo.begin(), taboo.end(), neighbour) == taboo.end()) {
                unvisitedNeighbours++;
                if (!checked) {
                    checked = true;
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
            std::cout << "unvisited neighbours: " << unvisitedNeighbours << " per " << neighbours.size() << " elements."
                      << std::endl;
            if (std::find(taboo.begin(), taboo.end(), result) == taboo.end()) {
                taboo.push_back(result);
            }
            if (taboo.size() == maxSize) {
                popFront(taboo);
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
            if (std::find(taboo.begin(), taboo.end(), result) == taboo.end()) {
                taboo.push_back(result);
            }
            if (taboo.size() == maxSize) {
                popFront(taboo);
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
    Puzzle bestPuzzle = puzzle;
    double bestScore = evaluate(puzzle);
    std::vector<Puzzle> neighbours;

    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < iterations; i++) {
        neighbours = getNeighbours(bestPuzzle);
        std::uniform_int_distribution<> distribution(0, neighbours.size() - 1);
        Puzzle tempPuzzle = neighbours[distribution(gen)];
        double tempScore = evaluate(tempPuzzle);
        if (tempScore > bestScore) {
            bestScore = tempScore;
            bestPuzzle = tempPuzzle;
        }
    }
    return bestPuzzle;
}


Puzzle annealing(const Puzzle &puzzle, int iterations, bool convergenceCurveSilence, bool printCallsSilence) {
    std::random_device rd;
    std::mt19937 gen(rd());
    int callAmount = 0;
    double result = evaluate(puzzle);
    Puzzle bestPuzzle = puzzle;
    Puzzle currentPosition = puzzle;

    for (int k = 0; k < iterations; k++) {
        std::vector<Puzzle> neighbours = getNeighbours(currentPosition);
        std::uniform_int_distribution<> distribution(0, neighbours.size() - 1);
        Puzzle neighbor = neighbours[distribution(gen)];
        double neighborScore = evaluate(neighbor);
        if (neighborScore > result) {
            result = neighborScore;
            bestPuzzle = neighbor;
            currentPosition = neighbor;
        } else {
            std::uniform_real_distribution<double> rand(0, 1);
            double temp = neighborScore - result;
            if (temp < 0) {
                temp = temp * -1;
            }
            double Tk = 1000.0 / (k + 0.01);
            if (rand(gen) < exp(-1 * temp / Tk)) {
                currentPosition = neighbor;
            }
        }
        if (!convergenceCurveSilence) {
            std::cout << k << "\t" << result << std::endl;
        }
    }
    if (!printCallsSilence) {
        std::cout << "evaluate was called: " << callAmount << " times." << std::endl;
    }
    return bestPuzzle;
}

Knapsack generateRandomKnapsack(int size) {
    std::vector<int> randomVector;
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(0, 1);
    for (int i = 0; i < size; i++) {
        randomVector.push_back(distr(eng));
    }
    Knapsack knapsack(randomVector);
    return knapsack;
}

Puzzle createRandomPuzzle(const Puzzle &puzzle) {
    int size = puzzle.getItemSet().size();
    auto ret = generateRandomKnapsack(size);


    return Puzzle(puzzle.getItemSet(), ret, puzzle.getCapacity());

}

int selectParent(std::vector<double> fitness) {
    double totalFitness = 0;
    for (double fitnes: fitness) {
        totalFitness += fitnes;
    }

    double randomFitness = ((double) rand() / (RAND_MAX)) * totalFitness;
    double currentFitness = 0;
    for (int i = 0; i < fitness.size(); i++) {
        currentFitness += fitness[i];
        if (currentFitness >= randomFitness) {
            return i;
        }
    }
    return 0;
}


Puzzle crossover(const Puzzle &parent1, const Puzzle &parent2) {
    double crossoverRate = 1;

    double randomNumber = ((double) rand() / (RAND_MAX));
    if (randomNumber < crossoverRate) {
        int crossoverPoint = rand() % parent1.getKnapsack().getBinary().size();
        std::vector<int> childBackpack;
#pragma omp parallel for
        for (int i = 0; i < crossoverPoint; i++) {
            childBackpack.push_back(parent1.getKnapsack().getBinary()[i]);
        }
#pragma omp parallel for
        for (int i = crossoverPoint; i < parent2.getKnapsack().getBinary().size(); i++) {
            childBackpack.push_back(parent2.getKnapsack().getBinary()[i]);
        }
        Puzzle child(parent1.getItemSet(), Knapsack(childBackpack), parent1.getCapacity());
        return child;
    }
    return parent1;
}


void mutate(Puzzle &puzzle) {
    double mutationRate = 0.05;
    std::vector<int> binary = puzzle.getKnapsack().getBinary();
#pragma omp parallel for
    for (int &i: binary) {
        double randomNumber = ((double) rand() / (RAND_MAX));
        if (randomNumber < mutationRate) {
            if (i == 1) {
                i = 0;
            } else {
                i = 1;
            }
        }
    }
    puzzle.setKnapsack(Knapsack(binary));
}


Puzzle geneticAlgorithm(const Puzzle &puzzle, int iterations, int populationSize) {
    std::vector<Puzzle> population;
    // Initialize the population
    for (int i = 0; i < populationSize; i++) {
        population.push_back(createRandomPuzzle(puzzle));
    }

    for (int i = 0; i < iterations; i++) {
        // Evaluate the fitness of each configuration
        std::vector<double> fitness;
        for (const auto &j: population) {
            fitness.push_back(evaluate(j));
        }

        // Select the fittest configurations for reproduction
        std::vector<Puzzle> parents;
        for (int j = 0; j < populationSize / 2; j++) {
            int parent1 = selectParent(fitness);
            int parent2 = selectParent(fitness);
            parents.push_back(population[parent1]);
            parents.push_back(population[parent2]);
        }

        // Perform crossover to generate new offspring
        std::vector<Puzzle> offspring;
        for (int j = 0; j < parents.size() - 1; j += 2) {
            Puzzle child = crossover(parents[j], parents[j + 1]);
            offspring.push_back(child);
        }

        // Perform mutation on the offspring
        for (auto &j: offspring) {
            mutate(j);
        }

        // Replace the current population with the new offspring
        population = offspring;
    }

    // Select the best puzzle
    Puzzle bestPuzzle = population[0];
    for (int i = 1; i < population.size(); i++) {
        if (evaluate(population[i]) > evaluate(bestPuzzle)) {
            bestPuzzle = population[i];
        }
    }
    return bestPuzzle;
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

const Knapsack &Puzzle::getKnapsack() const {
    return knapsack;
}

const std::vector<Item> &Puzzle::getItemSet() const {
    return itemSet;
}

int Puzzle::getCapacity() const {
    return capacity;
}

void Puzzle::setKnapsack(const Knapsack &knapsack) {
    Puzzle::knapsack = knapsack;
}

