#include "puzzleFunctions.hpp"

int main([[maybe_unused]] int argc, char **argv) {
    // example arguments:
    // ts C:\Users\au\CLionProjects\v3\data.json 1000 400
    // path needs to be absolute because reasons apparently
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

    auto start = std::chrono::steady_clock::now();
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
        std::cout << "bestPuzzle according to tabooPuzzle of " << iter << " iterations and " << maxSize << " maxSize:\n" << tabooPuzzle << std::endl;
        std::cout << "score: " << evaluate(tabooPuzzle);
    }
    if (method == "ts") {
        Puzzle tabooPuzzle = taboo(puzzle, iter, maxSize, true);
        std::cout << "bestPuzzle according to tabooPuzzle of " << iter << " iterations and " << maxSize << " maxSize:\n" << tabooPuzzle << std::endl;
        std::cout << "score: " << evaluate(tabooPuzzle);
    }
    if (method == "a") {
        Puzzle annealingPuzzle = annealing(puzzle, iter, false, false);
        std::cout << "bestPuzzle according to annealingPuzzle of " << iter << " iterations\n" << annealingPuzzle << std::endl;
        std::cout << "\nscore: " << evaluate(annealingPuzzle);
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "\nelapsed time: " << elapsed_seconds.count() << "s\n";

    return 0;
}
