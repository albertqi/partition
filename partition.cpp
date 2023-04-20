#include <stdio.h>
#include <math.h>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <thread>
#include <algorithm>

#define MAX_ITER 25000

thread_local std::mt19937 generator;

const long residue(const std::vector<long> &nums, const std::vector<short> &signs)
{
    if (nums.size() != signs.size())
    {
        throw std::invalid_argument("Array sizes must match.");
    }

    long res = 0;
    for (int i = 0; i < nums.size(); ++i)
    {
        res += nums[i] * signs[i];
    }

    return abs(res);
}

inline const long cooling_schedule(const int &iter)
{
    return pow(10, 10) * pow(0.8, iter / 300);
}

void process_prepartition(std::vector<long> &nums, const std::vector<int> &prepartition)
{
    if (nums.size() != prepartition.size())
    {
        throw std::invalid_argument("Array sizes must match.");
    }

    std::vector<long> new_nums(nums.size(), 0);
    for (int i = 0; i < nums.size(); ++i)
    {
        new_nums[prepartition[i]] += nums[i];
    }

    nums = new_nums;
}

const long kk(std::vector<long> nums)
{
    if (!nums.size())
    {
        throw std::invalid_argument("Array cannot be empty.");
    }

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

const long repeated_random(const std::vector<long> &nums, const int &max_iter = MAX_ITER)
{
    // Set up random generator.
    std::random_device random_dev;
    generator.seed(random_dev());
    std::uniform_real_distribution<double> unif_real(0.0, 1.0);

    // Initialize `solution`.
    std::vector<short> solution(nums.size(), 0);
    for (auto &x : solution)
    {
        x = unif_real(generator) <= 0.5 ? -1 : 1;
    }

    for (int i = 0; i < max_iter; ++i)
    {
        // Initialize `candidate`.
        std::vector<short> candidate(nums.size(), 0);
        for (auto &x : candidate)
        {
            x = unif_real(generator) <= 0.5 ? -1 : 1;
        }

        // Update `solution` if `candidate` is better.
        if (residue(nums, candidate) < residue(nums, solution))
        {
            solution = candidate;
        }
    }

    return residue(nums, solution);
}

const long simulated_annealing(const std::vector<long> &nums, const int &max_iter = MAX_ITER, const bool &is_stochastic = true)
{
    if (!nums.size())
    {
        throw std::invalid_argument("Array cannot be empty.");
    }

    if (nums.size() == 1)
    {
        return nums.front();
    }

    // Set up random generator.
    std::random_device random_dev;
    generator.seed(random_dev());
    std::uniform_real_distribution<double> unif_real(0.0, 1.0);
    std::uniform_int_distribution<int> unif_int(0, nums.size() - 1);

    // Initialize `solution`.
    std::vector<short> solution(nums.size(), 0);
    for (auto &x : solution)
    {
        x = unif_real(generator) <= 0.5 ? -1 : 1;
    }

    // Initialize `current`.
    std::vector<short> current(solution);

    for (int i = 0; i < max_iter; ++i)
    {
        // Initialize `neighbor`.
        std::vector<short> neighbor(current);

        // Pick two random indices.
        int a = unif_int(generator), b = unif_int(generator);
        while (a == b)
        {
            b = unif_int(generator);
        }

        // Update `neighbor`.
        neighbor[a] *= -1;
        neighbor[b] *= unif_real(generator) <= 0.5 ? -1 : 1;

        // Update `current`.
        if (residue(nums, neighbor) < residue(nums, current))
        {
            current = neighbor;
        }
        else if (is_stochastic && unif_real(generator) <= exp(-1.0 * (residue(nums, neighbor) - residue(nums, current)) / cooling_schedule(i)))
        {
            current = neighbor;
        }

        // Update `solution` if `current` is better.
        if (residue(nums, current) < residue(nums, solution))
        {
            solution = current;
        }
    }

    return residue(nums, solution);
}

inline const long hill_climbing(const std::vector<long> &nums, const int &max_iter = MAX_ITER)
{
    return simulated_annealing(nums, max_iter, false);
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

    // Open input file.
    std::ifstream file(input_file);

    // Parse input file.
    std::vector<long> nums(100, 0);
    for (int i = 0; i < 100; ++i)
    {
        file >> nums[i];
    }

    // Set up random generator.
    std::random_device random_dev;
    generator.seed(random_dev());
    std::uniform_int_distribution<int> unif_int(0, nums.size() - 1);

    // Initialize `prepartition`.
    std::vector<int> prepartition(nums.size(), 0);
    for (auto &x : prepartition)
    {
        x = unif_int(generator);
    }

    // Run specified algorithm and print residue.
    switch (alg)
    {
    case 0:
        printf("%ld\n", kk(nums));
        break;
    case 1:
        printf("%ld\n", repeated_random(nums));
        break;
    case 2:
        printf("%ld\n", hill_climbing(nums));
        break;
    case 3:
        printf("%ld\n", simulated_annealing(nums));
        break;
    case 11:
        process_prepartition(nums, prepartition);
        printf("%ld\n", repeated_random(nums));
        break;
    case 12:
        process_prepartition(nums, prepartition);
        printf("%ld\n", hill_climbing(nums));
        break;
    case 13:
        process_prepartition(nums, prepartition);
        printf("%ld\n", simulated_annealing(nums));
        break;
    }

    return 0;
}