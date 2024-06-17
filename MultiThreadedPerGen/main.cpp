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
#include <fstream>
#include <string>

#include "GASettings.h"
#include "./TargetCheck/TargetCheck.h"
#include "./GARunner/GARunner.h"

using namespace std::chrono;

const std::string LOG_FILE = "log.txt";

std::string getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::string dateTimeString = std::ctime(&now_c);
    dateTimeString.pop_back();
    return dateTimeString;
}

void fileWrite(const std::string& str) {
    std::ofstream ofs(LOG_FILE, std::ios::app);
    if (ofs.is_open()) {
        ofs << getCurrentDateTime() << "|" << str << std::endl; 
        ofs.close();
    } else {
        std::cerr << "Error opening file: " << LOG_FILE << std::endl;
    }
}

#include <thread>
int main(int argc, char* argv[]) {
    if(argc != 3) {}
        //return 1;

    const auto start = high_resolution_clock::now();
    const GASettings settings;
    TargetCheck target("std::cout << duration_cast<minutes>(runtime) << ' ' << duration_cast<seconds>(runtime) % 60 << ' ' << duration_cast<milliseconds>(runtime) % 1000 << ' ' << duration_cast<microseconds>(runtime) % 1000;");

    //int threads = std::stoi(argv[1]);
    //int evolutions = std::stoi(argv[2]);
    int threads = 32;
    int evolutions = 20'000;

    GARunner ga(settings, target, threads);
    ga.runFor(evolutions);

    const auto runtime = high_resolution_clock::now() - start;
    //std::cout << duration_cast<minutes>(runtime) << ' ' << duration_cast<seconds>(runtime) % 60 << ' ' << duration_cast<milliseconds>(runtime) % 1000 << ' ' << duration_cast<microseconds>(runtime) % 1000;
    std::cout << duration_cast<minutes>(runtime).count() << ' ' << duration_cast<seconds>(runtime).count()  % 60 << ' ' << duration_cast<milliseconds>(runtime).count()  % 1000 << ' ' << duration_cast<microseconds>(runtime).count()  % 1000;
    
    std::string durationStr = 
        "Threads: " + std::to_string(threads) + " Evolutions: " +  std::to_string(evolutions) + " Time: "
        + std::to_string(std::chrono::duration_cast<std::chrono::minutes>(runtime).count()) + "m "
        + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(runtime).count() % 60) + "s "
        + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(runtime).count() % 1000) + "ms "
        + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(runtime).count() % 1000) + "us";
    fileWrite(durationStr);

    std::cout.flush();
    exit(0);
}

//clang++ -std=c++20 -pthread -fdiagnostics-color=always -g     ConsoleRenderer/ConsoleRenderer.cpp     ThreadPool/ThreadPool.cpp     TargetCheck/TargetCheck.cpp     GARunner/GARunner.cpp     main.cpp     -o output