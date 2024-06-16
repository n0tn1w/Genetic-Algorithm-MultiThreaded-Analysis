#pragma once

#include <cassert>
#define gassert assert

#include "../TargetCheck/TargetCheck.h"
#include "../GASettings.h"
#include "../ConsoleRenderer/ConsoleRenderer.h"

struct Individual {
    std::string dna;
    int error = -1;
};

struct GARunner {

public:

    // Set _generation to be with size setting._population and be filled with targetCheck.getSize() long empty strings
    GARunner(const GASettings &settings, const TargetCheck &targetCheck);

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
    void crossover();
    // Get the percentage of elite
    int getEliteCount() const;
    // Random mix of dna of the 2 parents with a random
    Individual crossover(int left, int right) const;

    // Pick which element should be mutated
    void mutate();
    void mutate(Individual &individual);

    std::vector<Individual> _generation;

    GASettings _gaSettings;
    const TargetCheck& _checker;
    
    int _genPerSecond = 0;
    
    // How many percents of the final string is ready
    float _completed = 0.f;

    // Create new instance of renderer althougth 
    ConsoleRenderer _renderer;
    
    using UniformIntRange = std::uniform_int_distribution<int>;
    mutable std::mt19937 rng{ 42 };
    
    using UniformFloatRange = std::uniform_real_distribution<float>;

};


// #include "GARunner.h"

// #include <algorithm>
// #include <mutex>
// #include <string>
// #include <vector>
// #include <random>
// #include <chrono>
// #include <cassert>
// #include <cstring>
// #include <iostream>
// #include <iomanip>
// #include <sys/ioctl.h>
// #include <unistd.h>


// #include "../GASettings.h"
// #include "../ConsoleRenderer/ConsoleRenderer.cpp"
// #include "../TargetCheck/TargetCheck.h"

// using namespace std::chrono;
