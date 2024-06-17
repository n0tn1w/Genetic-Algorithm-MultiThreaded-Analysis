#pragma once

#include <cassert>
#include <thread>
#define gassert assert

#include "../TargetCheck/TargetCheck.h"
#include "../GASettings.h"
#include "../ConsoleRenderer/ConsoleRenderer.h"
#include "../ThreadPool/ThreadPool.h"

struct Individual {
    std::string dna;
    int error = -1;
};

struct GARunner {

public:

    // Set _generation to be with size setting._population and be filled with targetCheck.getSize() long empty strings
    GARunner(const GASettings &settings, const TargetCheck &targetCheck, size_t threadCount);

    // Numbers of generations it will be run for
    // If it is not ready by the final gen it will stop it
    void runFor(int generationCount);

private:

    void prepare();
    // Print on the console
    void render();

    // Create a newGeneration and swap it with _generation
    // It is created by taking the elite elements first 
    // and filling the rest with a crossover between random left and right element
    void static crossover(std::vector<Individual>& , std::mt19937& rng, size_t batch_start, size_t batch_end);
    // Get the percentage of elite
    int static getEliteCount();
    // Random mix of dna of the 2 parents with a random
    Individual static crossover(int left, int right, std::mt19937& rng);

    // Pick which element should be mutated
    static void mutate(std::vector<Individual>&, std::mt19937& rng, size_t batch_start, size_t batch_end);
    static void mutate(Individual &individual, std::mt19937& rng);

    static std::vector<Individual> _generation;

    static GASettings _gaSettings;
    static TargetCheck _checker;
    
    int _genPerSecond = 0;
    
    // How many percents of the final string is ready
    float _completed = 0.f;

    // Create new instance of renderer although 
    static ConsoleRenderer _renderer;
    
    using UniformIntRange = std::uniform_int_distribution<int>;
    //static std::mt19937 rng;

    using UniformFloatRange = std::uniform_real_distribution<float>;

    size_t _threadCount;
    std::vector<std::pair<int, int>> _threadsChunks;
    // For running multiple threads
    void static processBatchGeneration(std::vector<Individual>& threadGeneration, size_t, size_t);
    // For getting the elite inside of the _generation variables 
    void threadedPrepare(std::vector<Individual>& threadedGenerations);

    // fill _threadsChunks on def()
    void assignThreadChunkForThreadsValue();

    static ThreadPool _threadPool; 

};