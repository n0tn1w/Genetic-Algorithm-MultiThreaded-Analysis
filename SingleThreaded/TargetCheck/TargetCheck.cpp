#include "TargetCheck.h"


TargetCheck::TargetCheck(const std::string &desired) {
    char symbols[] = "_!@#$%^&*()<>[];:'\" ";
    _alphabet.reserve(256);
    for (int c = 'a'; c <= 'z'; c++) {
        _alphabet.push_back(char(c));
    }
    for (int c = 'A'; c <= 'Z'; c++) {
        _alphabet.push_back(char(c));
    }
    for (int c = '0'; c <= '9'; c++) {
        _alphabet.push_back(char(c));
    }

    for (int c = 0; c < 20; c++) {
        _alphabet.push_back(symbols[c]);
    }
    // Random picks a basePick
    _basePick = std::uniform_int_distribution<int>(0, int(_alphabet.size() - 1));
    const bool accepted = setTarget(desired);
    assert(accepted);
}

// Validate that the target can be done using only the alphabet letters
bool TargetCheck::setTarget(const std::string &desired) {
    for (int c = 0; c < int(desired.size()); c++) {
        if (_alphabet.find(desired[c]) == std::string::npos) {
            return false;
        }
    }
    _target = desired;
    return true;
}


int TargetCheck::getTargetSize() const {
    gassert(!_target.empty());
    return int(_target.size());
}

int TargetCheck::distance(const std::string &str) const {
    gassert(!_target.empty());
    if (str.size() != _target.size()) {
        gassert(str.size() != _target.size());
        return -1;
    }
    int sum = 0;
    for (int c = 0; c < int(_target.size()); c++) {
        const int diff = _target[c] - str[c];
        sum += diff * diff;
    }
    gassert(sum >= 0);
    return sum;
}

char TargetCheck::newBase(std::mt19937 &rng) const {
    return _alphabet[_basePick(rng)];
}

