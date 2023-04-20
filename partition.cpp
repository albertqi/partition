#include <stdio.h>
#include <math.h>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>

#define MAX_ITER 25000

std::mt19937 generator;

/**
 * Karmarkar-Karp algorithm.
 *
 * @param nums Array of numbers.
 * @return Residue of the solution.
 */
const long kk(std::vector<long> nums)
{
    // Check for empty array.
    if (!nums.size())
    {
        throw std::invalid_argument("Array cannot be empty.");
    }

    // Run the algorithm.
    std::make_heap(nums.begin(), nums.end());
    while (nums.size() >= 2)
    {
        std::pop_heap(nums.begin(), nums.end());
        const long a = nums.back();
        nums.pop_back();
        std::pop_heap(nums.begin(), nums.end());
        const long b = nums.back();
        nums.pop_back();
        nums.push_back(a - b);
        std::push_heap(nums.begin(), nums.end());
    }

    return nums.front();
}

/**
 * Preprocesses an input array with some prepartition.
 *
 * @param nums Array of numbers.
 * @param prepartition Prepartition of the array.
 * @return Preprocessed array.
 */
const std::vector<long> process_prepartition(const std::vector<long> &nums,
                                             const std::vector<int> &prepartition)
{
    // Check for mismatched array sizes.
    if (nums.size() != prepartition.size())
    {
        throw std::invalid_argument("Array sizes must match.");
    }

    // Create preprocessed array.
    std::vector<long> new_nums(nums.size(), 0);
    for (int i = 0; i < nums.size(); ++i)
    {
        new_nums[prepartition[i]] += nums[i];
    }

    return new_nums;
}

/**
 * Calculates the residue of a solution.
 *
 * @param nums Array of numbers.
 * @param solution Solution to the problem.
 * @param is_prepartitioned Whether the array is prepartitioned.
 * @return Residue of the solution.
 */
const long residue(const std::vector<long> &nums,
                   const std::vector<int> &solution,
                   const bool &is_prepartitioned)
{
    // Check for mismatched array sizes.
    if (nums.size() != solution.size())
    {
        throw std::invalid_argument("Array sizes must match.");
    }

    // Find residue with prepartitioned array.
    if (is_prepartitioned)
    {
        return kk(process_prepartition(nums, solution));
    }

    // Find residue with unprepartitioned array.
    long res = 0;
    for (int i = 0; i < nums.size(); ++i)
    {
        res += nums[i] * (solution[i] ? 1 : -1);
    }

    return abs(res);
}

/**
 * Repeated random algorithm.
 *
 * @param nums Array of numbers.
 * @param should_prepartition Whether to prepartition the array.
 * @return Residue of the solution.
 */
const long repeated_random(const std::vector<long> &nums,
                           const bool &should_prepartition)
{
    // Set up random generator.
    std::random_device random_dev;
    generator.seed(random_dev());
    std::uniform_int_distribution<int> unif_solution(0, should_prepartition ? nums.size() - 1 : 1);

    // Initialize `solution`.
    std::vector<int> solution(nums.size(), 0);
    for (auto &x : solution)
    {
        x = unif_solution(generator);
    }

    for (int i = 0; i < MAX_ITER; ++i)
    {
        // Initialize `candidate`.
        std::vector<int> candidate(nums.size(), 0);
        for (auto &x : candidate)
        {
            x = unif_solution(generator);
        }

        // Update `solution` if `candidate` is better.
        if (residue(nums, candidate, should_prepartition) <
            residue(nums, solution, should_prepartition))
        {
            solution = candidate;
        }
    }

    return residue(nums, solution, should_prepartition);
}

/**
 * Calculates the cooling schedule.
 *
 * @param iter Iteration number.
 * @return Temperature.
 */
inline const long cooling_schedule(const int &iter)
{
    return pow(10, 10) * pow(0.8, iter / 300);
}

/**
 * Simulated annealing algorithm.
 *
 * @param nums Array of numbers.
 * @param should_prepartition Whether to prepartition the array.
 * @param is_stochastic Whether to use simulated annealing or hill climbing.
 * @return Residue of the solution.
 */
const long simulated_annealing(const std::vector<long> &nums,
                               const bool &should_prepartition,
                               const bool &is_stochastic = true)
{
    // Check for empty array.
    if (!nums.size())
    {
        throw std::invalid_argument("Array cannot be empty.");
    }

    // Check for single element array.
    if (nums.size() == 1)
    {
        return nums.front();
    }

    // Set up random generator.
    std::random_device random_dev;
    generator.seed(random_dev());
    std::uniform_real_distribution<double> unif_real(0.0, 1.0);
    std::uniform_int_distribution<int> unif_solution(0, should_prepartition ? nums.size() - 1 : 1);
    std::uniform_int_distribution<int> unif_index(0, nums.size() - 1);

    // Initialize `solution`.
    std::vector<int> solution(nums.size(), 0);
    for (auto &x : solution)
    {
        x = unif_solution(generator);
    }

    // Initialize `current`.
    std::vector<int> current(solution);

    for (int i = 0; i < MAX_ITER; ++i)
    {
        // Initialize `neighbor`.
        std::vector<int> neighbor(current);

        // Generate a random neighbor.
        if (should_prepartition)
        {
            // Pick two random indices.
            int a = unif_index(generator), b = unif_index(generator);
            while (current[a] == b)
            {
                b = unif_index(generator);
            }

            // Update `neighbor`.
            neighbor[a] = b;
        }
        else
        {
            // Pick two random indices.
            int a = unif_index(generator), b = unif_index(generator);
            while (a == b)
            {
                b = unif_index(generator);
            }

            // Update `neighbor`.
            neighbor[a] = !neighbor[a];
            neighbor[b] = unif_solution(generator);
        }

        // Update `current`.
        if (residue(nums, neighbor, should_prepartition) <
            residue(nums, current, should_prepartition))
        {
            current = neighbor;
        }
        else if (is_stochastic && (unif_real(generator) <= exp(-1.0 * (residue(nums, neighbor, should_prepartition) - residue(nums, current, should_prepartition)) / cooling_schedule(i))))
        {
            current = neighbor;
        }

        // Update `solution` if `current` is better.
        if (residue(nums, current, should_prepartition) < residue(nums, solution, should_prepartition))
        {
            solution = current;
        }
    }

    return residue(nums, solution, should_prepartition);
}

/**
 * Hill climbing algorithm.
 *
 * @param nums Array of numbers.
 * @param should_prepartition Whether to prepartition the array.
 * @return Residue of the solution.
 */
inline const long hill_climbing(const std::vector<long> &nums,
                                const bool &should_prepartition)
{
    return simulated_annealing(nums, should_prepartition, false);
}

/**
 * Runs experiments.
 */
void run_experiments()
{
    // Set up random generator.
    std::random_device random_dev;
    generator.seed(random_dev());
    std::uniform_int_distribution<long> unif(1, pow(10, 12));

    for (int i = 0; i < 50; ++i)
    {
        // Initialize `nums`.
        std::vector<long> nums(100, 0);
        for (auto &x : nums)
        {
            x = unif(generator);
        }

        // Print results.
        printf("%ld\n", kk(nums));
        printf("%ld\n", repeated_random(nums, false));
        printf("%ld\n", hill_climbing(nums, false));
        printf("%ld\n", simulated_annealing(nums, false));
        printf("%ld\n", repeated_random(nums, true));
        printf("%ld\n", hill_climbing(nums, true));
        printf("%ld\n", simulated_annealing(nums, true));
    }
}

int main(int argc, char *argv[])
{
    // Check for valid command line arguments.
    if (argc != 4)
    {
        printf("Usage: ./partition [flag] [algorithm] [input_file]\n");
        return 1;
    }

    // Initialize command line arguments.
    double flag;
    int alg;
    std::string input_file;

    // Parse command line arguments.
    try
    {
        flag = std::stof(argv[1]);
        alg = std::stoi(argv[2]);
        input_file = argv[3];
    }
    catch (const std::invalid_argument &e)
    {
        printf("Usage: ./partition [flag] [algorithm] [input_file]\n");
        return 1;
    }

    // Run experiments.
    if (flag == 1)
    {
        run_experiments();
        return 0;
    }

    // Open input file.
    std::ifstream file(input_file);

    // Parse input file.
    std::vector<long> nums(100, 0);
    for (int i = 0; i < 100; ++i)
    {
        file >> nums[i];
    }

    // Run specified algorithm and print residue.
    switch (alg)
    {
    case 0:
        printf("%ld\n", kk(nums));
        break;
    case 1:
        printf("%ld\n", repeated_random(nums, false));
        break;
    case 2:
        printf("%ld\n", hill_climbing(nums, false));
        break;
    case 3:
        printf("%ld\n", simulated_annealing(nums, false));
        break;
    case 11:
        printf("%ld\n", repeated_random(nums, true));
        break;
    case 12:
        printf("%ld\n", hill_climbing(nums, true));
        break;
    case 13:
        printf("%ld\n", simulated_annealing(nums, true));
        break;
    }

    return 0;
}