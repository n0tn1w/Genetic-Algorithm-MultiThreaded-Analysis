#include <algorithm>
#include <mutex>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sys/ioctl.h>
#include <unistd.h>

#include "GASettings.h"
#include "./TargetCheck/TargetCheck.h"
#include "./GARunner/GARunner.h"

using namespace std::chrono;


#include <thread>
int main() {
    const auto start = high_resolution_clock::now();
    const GASettings settings;
    TargetCheck target("std::cout << duration_cast<minutes>(runtime) << ' ' << duration_cast<seconds>(runtime) % 60 << ' ' << duration_cast<milliseconds>(runtime) % 1000 << ' ' << duration_cast<microseconds>(runtime) % 1000;");
    GARunner ga(settings, target);

    ga.runFor(20'000);

    const auto runtime = high_resolution_clock::now() - start;
    //std::cout << duration_cast<minutes>(runtime) << ' ' << duration_cast<seconds>(runtime) % 60 << ' ' << duration_cast<milliseconds>(runtime) % 1000 << ' ' << duration_cast<microseconds>(runtime) % 1000;
    return 0;
}