#pragma once

#include <string>
#include <algorithm>
#include <cassert>
#include <random>

#define gassert assert

struct TargetCheck {

public:
    TargetCheck(const std::string &desired);

    // Get target len
    int getTargetSize() const;

    // Get the difference between the str and the target string
    int distance(const std::string &str) const;

    // mt19937 is a pseudo random algorithm
    // function will take a random value from the alphabet 
    char newBase(std::mt19937 &rng) const;

private:
    bool setTarget(const std::string &desired);

    std::string _target;
    std::string _alphabet;
    mutable std::uniform_int_distribution<int> _basePick;
};