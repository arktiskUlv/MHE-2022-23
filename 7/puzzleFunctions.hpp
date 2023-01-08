#import "puzzleObjects.hpp"

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

void popFront(std::vector<Puzzle> &v) {
    if (!v.empty()) {
        v.erase(v.begin());
    }
}

Puzzle taboo(const Puzzle &puzzle, int iterations, int maxSize, bool silent) {
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

        for (const Puzzle &neighbour: neighbours) {
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
            std::cout << "unvisited neighbours: " << unvisitedNeighbours << " per " << neighbours.size() << " elements."
                      << std::endl;
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

Puzzle annealing(const Puzzle& puzzle, int iterations, bool convergenceCurveSilence, bool printCallsSilence) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    int callAmount = 0;
    double result = evaluate(puzzle);
    Puzzle bestPuzzle = puzzle;
    Puzzle currentPosition = puzzle;

    for (int k = 0; k < iterations; k++) {
        std::vector<Puzzle> neighbours = getNeighbours(currentPosition);

        std::uniform_int_distribution<> distr(0, neighbours.size() - 1); // define the range
        Puzzle neighbour = neighbours[distr(gen)];

        double newResult = evaluate(neighbour);
        if (newResult > result) {
            result = newResult;
            bestPuzzle = neighbour;
            currentPosition = neighbour;

        } else {
            std::uniform_real_distribution<double> rand(0, 1);
            double temp = newResult - result;
            if (temp < 0) {
                temp = temp * -1;
            }
            double Tk = 1000.0 / (k + 0.01);
            if (rand(gen) < exp(-1 * temp / Tk)) {

                currentPosition = neighbour;

            }

        }
        if (convergenceCurveSilence) {
            std::cout << k << "\t" << result << std::endl;
        }

    }
    if (printCallsSilence) {
        std::cout << "evaluate was called: " << callAmount << " times." << std::endl;
    }
    return bestPuzzle;
}
