#pragma once

#include <chrono>
#include <algorithm>
#include <string>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <cmath>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std::chrono;

struct ConsoleRenderer {

public: 

    ConsoleRenderer() = default;

    void clear();
    void renderText(const char *text);
    void renderText(int value);
    void newLine();

private:
    int averageFPS = 0;
    int lastSecondFrames = 0;
    int frameCounter = 0;
    int clearCount = 0;
    int lineLength = 0;

    high_resolution_clock::time_point lastFrame;
    high_resolution_clock::time_point lastSecond;


};